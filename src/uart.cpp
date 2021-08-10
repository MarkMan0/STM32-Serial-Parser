/** @file uart.cpp
 * Contains code to initialize the uart peripherial
 */

#include "uart.h"

const Uart::msg_t Uart::kEmptyMsg{ 0 };

Uart::Uart() {
  tx_buff_.reset();
  rx_buff_.reset();
}


void Uart::tick() {
  auto ptr = tx_buff_.get_next_occupied();
  if (ptr == nullptr) return;
  if (transmit(ptr->data())) {
    tx_buff_.pop();
  }
}

void Uart::init() {
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
  if (HAL_UART_Init(&huart_) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief Device specific UART initialization.
 * Called by HAL_UART_Init(...)
 *
 * @param uart uart handle
 */
void HAL_UART_MspInit(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART2) {
    GPIO_InitTypeDef gpio{ 0 };

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Alternate = GPIO_AF7_USART2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pin = GPIO_PIN_15 | GPIO_PIN_2;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio);

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

    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    HAL_DMA_Init(&dma_tx);

    dma_rx.Instance = DMA1_Channel6;
    dma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_rx.Init.MemInc = DMA_MINC_ENABLE;
    dma_rx.Init.Mode = DMA_CIRCULAR;
    dma_rx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_rx.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_rx);

    __HAL_LINKDMA(uart, hdmatx, dma_tx);
    __HAL_LINKDMA(uart, hdmarx, dma_rx);

    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
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
  start = (uart2.dma_state_.prevCNDTR < uart2.kDmaRxBuffSize) ? (uart2.kDmaRxBuffSize - uart2.dma_state_.prevCNDTR) : 0;

  if (uart2.dma_state_.flag) {
    /* Timeout event */
    /* Determine new data length based on previous DMA_CNDTR value:
     *  If previous CNDTR is less than DMA buffer size: there is old data in DMA buffer (from previous timeout) that has
     * to be ignored. If CNDTR == DMA buffer size: entire buffer content is new and has to be processed.
     */
    length = (uart2.dma_state_.prevCNDTR < uart2.kDmaRxBuffSize) ? (uart2.dma_state_.prevCNDTR - currCNDTR) :
                                                                   (uart2.kDmaRxBuffSize - currCNDTR);
    uart2.dma_state_.prevCNDTR = currCNDTR;
    uart2.dma_state_.flag = 0;
  } else {
    /* DMA Rx Complete event */
    length = uart2.kDmaRxBuffSize - start;
    uart2.dma_state_.prevCNDTR = uart2.kDmaRxBuffSize;
  }

  // get pointer to buffer
  auto ptr = uart2.rx_buff_.get_next_free();

  if (ptr == nullptr) {
    return;
  }
  /* Copy and Process new data */
  for (i = 0, pos = start; i < length; ++i, ++pos) {
    (*ptr)[i] = uart2.dma_rx_buff_[pos];
  }
  uart2.rx_buff_.push();
}

void Uart::start_listen() {
  /**Enable USART 2 IDLE interrupt and register callback */
  SET_BIT(USART2->CR1, USART_CR1_IDLEIE);
  HAL_UART_RegisterCallback(&uart2.huart_, HAL_UART_RX_COMPLETE_CB_ID, HAL_UART_RxCpltCallback);
  /** Start receiving */
  HAL_UART_Receive_DMA(&huart_, dma_rx_buff_.data(), dma_rx_buff_.size());
  /** Enable DMA1_6 interrupt */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}



Uart uart2;
