#ifndef UART_H_
#define UART_H_

/** @file uart.h
 * Class to encapsulate helper functions for uart operations
 */

#include "main.h"
#include <cstring>
#include "ring_buffer.h"
#include <array>
#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @brief Encapsulates UART2
 * Messages are put into a ring buffer, and transmitted one after another
 * Transmission is also possible directly, without the use of the Ring buffer
 * Direct transmission can also be done using DMA
 *
 */
class Uart {
public:
  static constexpr size_t kTxBufferSize = 10,  //!< The size of the transmission ring buffer
      kRxBufferSize = 10,                      //!< The size of the Rx Ring buffer
      kMsgLen = 30,                            //!< Max lenth of a message to transmit
      kDmaRxBuffSize = 30;                     //!< RX DMA buffer size
  using msg_t = std::array<char, kMsgLen>;     //!< message type alias

  /**
   * @brief handle to uart
   *
   */
  UART_HandleTypeDef huart_;


  /**
   * @brief Construct a new Uart object
   * Resets the buffers, BUT doesn't initialize the Uart peripherial
   */
  Uart();

  /**
   * @brief Periodic task which should be called whenever possible
   * Handles transmitting from buffer
   */
  void tick();

  /**
   * @brief puts the message pointed to by \p buff to the tx queue
   *
   * @param buff pointer to data to be transmitted
   * @param num length of data
   * @return true on succes, false if couldn't enqueue
   */
  bool send_queue(const char* buff, size_t num, bool from_isr = false);

  /**
   * @brief put a known-size array into send queue
   *
   * @tparam N automatically deduced size of array
   * @return true on success
   */
  template <size_t N>
  bool send_queue(const char (&arr)[N], bool from_isr = false) {
    return send_queue(arr, N, from_isr);
  }

  /**
   * @brief put a null-terminated array into send queue
   *
   * @param data pointer to null terminated char array
   * @return true on success
   */
  bool send_queue(const char* data, bool from_isr = false) {
    return send_queue(data, strlen(data), from_isr);
  }

  /**
   * @brief Starts listening for incoming messages
   *
   */
  void start_listen();

  /**
   * @brief initializes the UART peripherial
   *
   */
  void init();

  /**
   * @brief Check if RX buffer has data
   *
   * @return true id RX buffer is not empty
   */
  bool has_message() const {
    return !rx_buff_.is_empty();
  }

  /**
   * @brief Is the RX buffer full?
   *
   * @return true if rx buffer is full
   */
  bool is_rx_full() const {
    return rx_buff_.is_full();
  }

  /**
   * @brief Get the next message from buffer
   * @details return const reference to a message, or to an internal buffer, which is all 0
   * @return const reference to message, or to an internal buffer
   */
  const msg_t& get_message() const {
    if (!has_message()) return kEmptyMsg;
    return *(rx_buff_.get_next_occupied());
  }

  /**
   * @brief Calls pop() on rx_buff_
   *
   */
  void pop_rx() {
    rx_buff_.pop();
  }

  /**
   * @brief Non blocking, immediate transmission via UART.
   *
   * @param data pointer to data to be transmitted
   * @param len number of bytes to be transmitted
   * @return true on success
   */
  bool transmit(uint8_t* data, size_t len) {
    return HAL_UART_Transmit(&huart_, data, len, 100) == HAL_OK;
  }
  /**
   * @brief Non blocking, immediate Used to transmit char[] arrays what are declared by the user.
   *
   * @tparam N automatically deduces by the compiler
   * @return true on success
   * @see transmit(uint8_t* data, size_t len)
   */
  template <size_t N>
  bool transmit(const char (&arr)[N]) {
    return transmit(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
  }
  /**
   * @brief Non blocking, immediate transmission of a null-terminated array
   *
   * @param data pointer to null terminated char array
   * @return true on success
   */
  bool transmit(const char* data) {
    return transmit(reinterpret_cast<uint8_t*>(const_cast<char*>(data)), strlen(data));
  }

  /**
   * @brief Transmits data immediately using DMA
   *
   * @param data pointer to data
   * @param len length of data
   * @return true on success
   * @see transmit(uint8_t* data, size_t len)
   */
  bool transmit_DMA(uint8_t* data, size_t len) {
    return HAL_UART_Transmit_DMA(&huart_, data, len) == HAL_OK;
  }
  /**
   * @brief transmit a char[] array immediately using DMA
   *
   * @tparam N lenght of array, deduced by the compiler
   * @return true on success
   * @see transmit(const char (&arr)[N])
   */
  template <size_t N>
  bool transmit_DMA(const char (&arr)[N]) {
    return transmit_DMA(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
  }
  /**
   * @brief Transmits a null-terminated array immediately using DMA
   *
   * @param data pointer to null-terminated array
   * @return true on success
   * @see transmit(char* data)
   */
  bool transmit_DMA(const char* data) {
    return transmit_DMA(reinterpret_cast<uint8_t*>(const_cast<char*>(data)), strlen(data));
  }
  /**
   * @brief Called on UART IDLE interrupt, starts countdown
   *
   */
  void on_idle_ISR() {
    dma_state_.countdown = 2;
  }

  /**
   * @brief Called from SYSTICK ISR, counts down, on zero calls callback
   * @see HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
   * @see dma_state_
   */
  void on_systick_ISR() {
    if (dma_state_.countdown) {
      --dma_state_.countdown;
      if (dma_state_.countdown == 0) {
        dma_state_.flag = true;
        HAL_NVIC_SetPendingIRQ(DMA1_Channel6_IRQn);
      }
    }
  }

  void on_DMA_ISR() {
    if (dma_state_.flag) {
      huart_.hdmarx->XferCpltCallback(huart_.hdmarx);
    }
  }

private:
  RingBuffer<msg_t, kTxBufferSize> tx_buff_;         //!< Tx ring buffer
  RingBuffer<msg_t, kRxBufferSize> rx_buff_;         //!< RX Ring buffer
  std::array<uint8_t, kDmaRxBuffSize> dma_rx_buff_;  //!< DMA buffer

  /**
   * @brief Callback for DMA complete ISR
   *
   * @param huart uart handle
   */
  friend void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);

  /**
   * @var dma_state_
   * @brief Anonymous struct to track IDLE line
   * On IDLE interrupt the countdown is set to a positive value.
   * Countdown is decremented every ms, when reaches 0, HAL_UART_RxCpltCallback is called
   */
  struct {
    volatile bool flag{ false };
    volatile uint16_t countdown{ 0 };
    uint16_t prevCNDTR{ kDmaRxBuffSize };
  } dma_state_;

  /**
   * @brief When getMessage is called and the buffer is empty, referenci is returned to this variable.
   *
   */
  static const msg_t kEmptyMsg;
};



inline bool Uart::send_queue(const char* buff, size_t num, bool from_isr) {
  if (num > kMsgLen || num == 0) return false;
  if (!from_isr) {
    vPortEnterCritical();
  }
  auto buff_ptr = tx_buff_.get_next_free();
  if (!from_isr) {
    vPortExitCritical();
  }
  if (buff_ptr == nullptr) return false;
  memset(buff_ptr->data(), 0, buff_ptr->size());
  memcpy(buff_ptr->data(), buff, num);
  tx_buff_.push();
  extern SemaphoreHandle_t uart_tx_sem;
  xSemaphoreGiveFromISR(uart_tx_sem, NULL);
  return true;
}

extern Uart uart2;

#endif  // UART_H_
