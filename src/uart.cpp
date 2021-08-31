/** @file uart.cpp
 * Contains code to initialize the uart peripherial
 */

#include "uart.h"
#include "pin_api.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "utils.h"

// Static members
const Uart::msg_t Uart::kEmptyMsg{ 0 };

void Uart::uart_transmit_task(void* arg) {
  while (true) {
    if (xSemaphoreTake(uart2.tx_semaphore_, portMAX_DELAY)) {
      uart2.tick();
    }
  }
}

// Member function definitions

Uart::Uart() {
  tx_buff_.reset();
  rx_buff_.reset();
}


void Uart::tick() {
  while (!tx_buff_.is_empty()) {
    auto ptr = tx_buff_.get_next_occupied();
    if (ptr == nullptr) return;
    transmit("echo: ");
    if (transmit(reinterpret_cast<uint8_t*>(ptr->data()), strnlen(ptr->data(), kMsgLen))) {
      transmit("\n");
      tx_buff_.pop();
    }
  }
}

void Uart::init_peripherals() {
  huart_.Instance = USART2;
  huart_.Init.BaudRate = 115200;
  huart_.Init.WordLength = UART_WORDLENGTH_8B;
  huart_.Init.StopBits = UART_STOPBITS_1;
  huart_.Init.Parity = UART_PARITY_NONE;
  huart_.Init.Mode = UART_MODE_TX_RX;
  huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  utils::hal_wrap(HAL_UART_Init(&huart_));
}

/**
 * @brief Device specific UART initialization.
 * Called by HAL_UART_Init(...)
 *
 * @param uart uart handle
 */
void HAL_UART_MspInit(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART2) {
    __HAL_RCC_USART2_CLK_ENABLE();

    pins::uart2_rx.init();
    pins::uart2_tx.init();

    __HAL_RCC_DMA1_CLK_ENABLE();

    static DMA_HandleTypeDef dma_tx, dma_rx;

    dma_tx.Instance = DMA1_Channel7;
    dma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_tx.Init.MemInc = DMA_MINC_ENABLE;
    dma_tx.Init.Mode = DMA_NORMAL;
    dma_tx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_tx.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&dma_tx) != HAL_OK) {
      uart2.transmit("HAL DMA Init failed for tx");
    }

    dma_rx.Instance = DMA1_Channel6;
    dma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_rx.Init.MemInc = DMA_MINC_ENABLE;
    dma_rx.Init.Mode = DMA_CIRCULAR;
    dma_rx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_rx.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&dma_rx) != HAL_OK) {
      uart2.transmit("HAL DMA Init failed for rx");
    }

    __HAL_LINKDMA(uart, hdmatx, dma_tx);
    __HAL_LINKDMA(uart, hdmarx, dma_rx);
  }
}

/**
 * @brief UART receive complete callback, called through DMA handle by HAL
 * Based on code from https://github.com/akospasztor/stm32-dma-uart
 * @param huart
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  uint16_t i, pos, start, length;
  uint16_t currCNDTR = __HAL_DMA_GET_COUNTER(huart->hdmarx);

  /* Ignore IDLE Timeout when the received characters exactly filled up the DMA buffer and DMA Rx Complete IT is
   * generated, but there is no new character during timeout */
  if (uart2.dma_state_.flag && currCNDTR == uart2.kDmaRxBuffSize) {
    uart2.dma_state_.flag = 0;
    return;
  }

  /* Determine start position in DMA buffer based on previous CNDTR value */
  start = 0;

  if (uart2.dma_state_.flag) {
    /* Timeout event */
    /* Determine new data length based on previous DMA_CNDTR value:
     *  If previous CNDTR is less than DMA buffer size: there is old data in DMA buffer (from previous timeout) that has
     * to be ignored. If CNDTR == DMA buffer size: entire buffer content is new and has to be processed.
     */
    length = uart2.kDmaRxBuffSize - currCNDTR;
    uart2.dma_state_.flag = 0;
  } else {
    /* DMA Rx Complete event */
    length = uart2.kDmaRxBuffSize;
  }

  // get pointer to buffer
  auto ptr = uart2.rx_buff_.get_next_free();

  if (ptr != nullptr) {
    /* Copy and Process new data */
    for (i = 0, pos = start; i < length && i < uart2.kMsgLen - 1; ++i, ++pos) {
      (*ptr)[i] = uart2.dma_rx_buff_[pos];
    }
    (*ptr)[i] = '\0';
    uart2.rx_buff_.push();

    /** Give rx semaphore */
    xSemaphoreGiveFromISR(uart2.rx_semaphore_, NULL);
    if (!uart2.rx_buff_.is_full()) {
      // uart2.send_queue("ok", true);
    }
  }
  /* Fast reset the DMA*/
  huart->hdmarx->Instance->CCR &= ~DMA_CCR_EN;
  huart->hdmarx->Instance->CNDTR = uart2.dma_rx_buff_.size();
  huart->hdmarx->Instance->CCR |= DMA_CCR_EN;
}

void Uart::begin() {
  /** create sempahores and start tasks*/
  rx_semaphore_ = xSemaphoreCreateCounting(kRxBufferSize, 0);
  tx_semaphore_ = xSemaphoreCreateCounting(kTxBufferSize, 0);

  uart_send_task_handle_ = osThreadNew(Uart::uart_transmit_task, NULL, &kUartSendTaskAttr);

  /** Start transmit DMA IRQ*/
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

  /** Start USART2 IRQ*/
  HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  /**Enable USART 2 IDLE interrupt and register callback */
  SET_BIT(USART2->CR1, USART_CR1_IDLEIE);
  utils::hal_wrap(HAL_UART_RegisterCallback(&uart2.huart_, HAL_UART_RX_COMPLETE_CB_ID, HAL_UART_RxCpltCallback));

  /** Start receiving */
  utils::hal_wrap(HAL_UART_Receive_DMA(&huart_, dma_rx_buff_.data(), dma_rx_buff_.size()));

  /** Enable DMA1_6 interrupt */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

Uart::msg_t* Uart::get_next_free_or_yield(uint32_t timeout) {
  bool exit = false;
  msg_t* ptr{ nullptr };
  const uint32_t start = HAL_GetTick();
  do {
    vPortEnterCritical();
    ptr = tx_buff_.get_next_free();
    vPortExitCritical();

    if (ptr == nullptr && HAL_GetTick() - start < timeout) {
      taskYIELD();
      exit = true;
    } else {
      exit = true;
    }
  } while (!exit);

  return ptr;
}

bool Uart::send_queue(const char* buff, size_t num, bool from_isr) {
  if (num > (kMsgLen - 1) || num == 0) return false;

  msg_t* buff_ptr{ nullptr };

  if (from_isr) {
    buff_ptr = tx_buff_.get_next_free();
  } else {
    buff_ptr = get_next_free_or_yield(5);
  }

  if (!buff_ptr) {
    return false;
  }

  (*buff_ptr)[num] = '\0';
  memcpy(buff_ptr->data(), buff, num);
  tx_buff_.push();
  /** Give TX semaphore */
  xSemaphoreGiveFromISR(tx_semaphore_, NULL);
  return true;
}


Uart uart2;
