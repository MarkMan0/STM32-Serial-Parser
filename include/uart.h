#ifndef UART_H_
#define UART_H_

/** @file uart.h
 * Class to encapsulate helper functions for uart operations
 */

#include "main.h"
#include <cstring>
#include "ring_buffer.h"
#include <array>

/**
 * @brief Encapsulates UART2
 * Messages are put into a ring buffer, and transmitted one after another
 * Transmission is also possible directly, without the use of the Ring buffer
 * Direct transmission can also be done using DMA
 *
 */
class Uart {
private:
  static constexpr size_t kTxBufferSize = 10,  //!< The size of the transmission ring buffer
      kRxBufferSize = 10,                      //!< The size of the Rx Ring buffer
      kMsgLen = 30,                            //!< Max lenth of a message to transmit
      kDmaRxBuffSize = 30;                     //!< RX DMA buffer size

  using msg_t = std::array<char, kMsgLen>;  //!< message type alias

  RingBuffer<msg_t, kTxBufferSize> tx_buff_;  //!< Tx ring buffer
  RingBuffer<msg_t, kRxBufferSize> rx_buff_;  //!< RX Ring buffer

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

public:
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
  bool send_queue(const char* buff, size_t num);

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
   * @brief Non blocking, immediate transmission via UART.
   *
   * @param data pointer to data to be transmitted
   * @param len number of bytes to be transmitted
   * @return HAL_StatusTypeDef HAL_OK on success
   */
  HAL_StatusTypeDef transmit(uint8_t* data, size_t len);
  /**
   * @brief Non blocking, immediate Used to transmit char[] arrays what are declared by the user.
   *
   * @tparam N automatically deduces by the compiler
   * @return HAL_StatusTypeDef HAL_OK on succes
   * @see transmit(uint8_t* data, size_t len)
   */
  template <size_t N>
  HAL_StatusTypeDef transmit(const char (&arr)[N]);
  /**
   * @brief Non blocking, immediate transmission of a null-terminated array
   *
   * @param data pointer to null terminated char array
   * @return HAL_StatusTypeDef HAL_OK on success
   */
  HAL_StatusTypeDef transmit(char* data);

  /**
   * @brief Transmits data immediately using DMA
   *
   * @param data pointer to data
   * @param len length of data
   * @return HAL_StatusTypeDef
   * @see transmit(uint8_t* data, size_t len)
   */
  HAL_StatusTypeDef transmit_DMA(uint8_t* data, size_t len);
  /**
   * @brief transmit a char[] array immediately using DMA
   *
   * @tparam N lenght of array, deduced by the compiler
   * @return HAL_StatusTypeDef
   * @see transmit(const char (&arr)[N])
   */
  template <size_t N>
  HAL_StatusTypeDef transmit_DMA(const char (&arr)[N]);
  /**
   * @brief Transmits a null-terminated array immediately using DMA
   *
   * @param data pointer to null-terminated array
   * @return HAL_StatusTypeDef
   * @see transmit(char* data)
   */
  HAL_StatusTypeDef transmit_DMA(char* data);
  /**
   * @brief Called on UART IDLE interrupt, starts countdown
   *
   */
  void onIdleISR() {
    dma_state_.countdown = 5;
  }

  /**
   * @brief Called from SYSTICK ISR, counts down, on zero calls callback
   * @see HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
   * @see dma_state_
   */
  void onSysTickISER() {
    if (dma_state_.countdown) {
      --dma_state_.countdown;
      if (dma_state_.countdown == 0) {
        dma_state_.flag = true;
        huart_.hdmarx->XferCpltCallback(huart_.hdmarx);
      }
    }
  }
};


inline Uart::Uart() {
  tx_buff_.reset();
  rx_buff_.reset();
}


inline void Uart::tick() {
  auto ptr = tx_buff_.getNextOccupied();
  if (ptr == nullptr) return;
  if (transmit(ptr->data()) == HAL_OK) {
    tx_buff_.pop();
  }
}


inline bool Uart::send_queue(const char* buff, size_t num) {
  if (num > kMsgLen) return false;
  auto buff_ptr = tx_buff_.getNextFree();
  if (buff_ptr == nullptr) return false;
  memset(buff_ptr->data(), 0, buff_ptr->size());
  memcpy(buff_ptr->data(), buff, num);
  tx_buff_.push();
  return true;
}

inline HAL_StatusTypeDef Uart::transmit(uint8_t* data, size_t len) {
  return HAL_UART_Transmit(&huart_, data, len, 500);
}

template <size_t N>
inline HAL_StatusTypeDef Uart::transmit(const char (&arr)[N]) {
  if (N == 0) return HAL_OK;
  return transmit(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
}

inline HAL_StatusTypeDef Uart::transmit(char* data) {
  return transmit(reinterpret_cast<uint8_t*>(data), strlen(data));
}

inline HAL_StatusTypeDef Uart::transmit_DMA(uint8_t* data, size_t len) {
  const auto start = HAL_GetTick();
  while (HAL_UART_Transmit_DMA(&huart_, data, len) != HAL_OK) {
    if (HAL_GetTick() > (start + 100)) {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

template <size_t N>
inline HAL_StatusTypeDef Uart::transmit_DMA(const char (&arr)[N]) {
  if (N == 0) return HAL_OK;
  return transmit_DMA(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
}

inline HAL_StatusTypeDef Uart::transmit_DMA(char* data) {
  return transmit_DMA(reinterpret_cast<uint8_t*>(data), strlen(data));
}

extern Uart uart2;

#endif  // UART_H_
