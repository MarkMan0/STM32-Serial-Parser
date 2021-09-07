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
#include "cmsis_os.h"
#include "semphr.h"
#include "utils.h"
#include "string.h"
#include <type_traits>
#include <cstdarg>

/**
 * @brief Encapsulates UART2
 * Messages are put into a ring buffer, and transmitted one after another
 * Transmission is also possible directly, without the use of the Ring buffer
 * Direct transmission can also be done using DMA
 *
 */
class Uart {
public:
  static constexpr size_t kTxBufferSize = 5,  //!< The size of the transmission ring buffer
      kRxBufferSize = 5,                      //!< The size of the Rx Ring buffer
      kMsgLen = 30,                           //!< Max lenth of a message to transmit
      kDmaRxBuffSize = 30;                    //!< RX DMA buffer size
  using msg_t = std::array<char, kMsgLen>;    //!< message type alias

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
   * @param from_isr set to true if function is called from ISR
   * @return true on succes, false if couldn't enqueue
   */
  bool send_queue(const char* buff, size_t num, bool from_isr = false);

  /**
   * @brief put a known-size array into send queue
   *
   * @tparam N automatically deduced size of array
   * @param from_isr set to true if function is called from ISR
   * @return true on success
   */
  template <size_t N, class T>
  bool send_queue(const char (&arr)[N], T from_isr = false) {
    if constexpr (std::is_same<T, bool>::value) {
      return send_queue(arr, N, from_isr);
    } else {
      /*
       * In case when this is called like this:
       * char foo[30];
       * ....
       * send_queue(foo, 15);  // want to send 15 chars
       * the template will be:
       * send_queue(const char &arr[30], 15)
       * and 15 will be converted to bool
       * so this is called: send_queue(arr, 30, true)
       * Which is not what the user wanted
       * The if constexpr solves this issue, from_isr is false.
       *
       * If the user would have give from_isr like this: send_queue(foo, 15, true/false)
       * the send_queue(const char*, size_t, bool) would be called
       */
      return send_queue(arr, strnlen(arr, N), false);
    }
  }

  /**
   * @brief put a null-terminated array into send queue
   *
   * @param data pointer to null terminated char array
   * @param from_isr set to true if function is called from ISR
   * @return true on success
   */
  bool send_queue(const char* data, bool from_isr = false) {
    return send_queue(data, strlen(data), from_isr);
  }

  /**
   * @brief Starts the ISRs, the freeRTOS tasks and begins transmission/reception
   *
   */
  void begin();

  /**
   * @brief initializes the UART peripherial
   *
   */
  void init_peripherals();

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
   * @brief Print formatted data using queue
   * @details Use in ISR, will not yield and wait if the buffer is full
   * @see send_queue
   * @param fmt printf-style format
   * @param ... data
   * @return true on success
   */
  bool printf_isr(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto res = vprintf(true, fmt, args);
    va_end(args);
    return res;
  }

  /**
   * @brief Print formatted data using queue
   * @details Will yield to OS if the buffer is full
   * @see send_queue
   * @param fmt printf-style format
   * @param ... data
   * @return true on success
   */
  bool printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto res = vprintf(false, fmt, args);
    va_end(args);
    return res;
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
        // we cant call XferCpltCallback, because of FreeRTOS, this ISR's priority is too high
        HAL_NVIC_SetPendingIRQ(DMA1_Channel6_IRQn);
      }
    }
  }

  /**
   * @brief used to call XferCpltCallback when flag is set
   *
   * calling from HAL tick ISR doesn't work with freertos, it's priority is too high
   */
  void on_DMA_ISR() {
    if (dma_state_.flag) {
      huart_.hdmarx->XferCpltCallback(huart_.hdmarx);
    }
  }

  /**
   * @brief GcodeParser needs direct access to rx semaphore
   *
   */
  friend class GcodeParser;

private:
  RingBuffer<msg_t, kTxBufferSize> tx_buff_;         //!< Tx ring buffer
  RingBuffer<msg_t, kRxBufferSize> rx_buff_;         //!< RX Ring buffer
  std::array<uint8_t, kDmaRxBuffSize> dma_rx_buff_;  //!< DMA buffer
  SemaphoreHandle_t rx_semaphore_, tx_semaphore_;    //!< RTOS semaphores
  osThreadId_t uart_send_task_handle_;               //!< RTOS handle to task
  const osThreadAttr_t kUartSendTaskAttr{ utils::create_thread_attr("uart_send", 128 * 4, osPriorityAboveNormal7) };

  /**
   * @brief Wrapper for vsnprintf, prints directly into the buffer
   *
   * @param from_isr when true, will not yield if buffer is full
   * @param fmt format
   * @param args args obtained by va_start()
   * @return true on success
   */
  bool vprintf(bool from_isr, const char* fmt, va_list args);

  /**
   * @brief Get the next free in buffer, block and yield if can't
   *
   * @param timeout max time to wait in ms
   * @return msg_t*
   */
  msg_t* get_next_free_or_yield(uint32_t timeout);

  /**
   * @brief Task to transmit from ringBuffer
   *
   * Task waits for sempahore, and upon acquiring, transmits all available data from buffer
   */
  static void uart_transmit_task(void*);

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


extern Uart uart2;

#endif  // UART_H_
