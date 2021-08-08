#include "uart.h"

/** @file uart.cpp
 * Contains code to initialize the uart peripherial
 */

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

    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    HAL_DMA_Init(&dma_rx);

    __HAL_LINKDMA(uart, hdmatx, dma_tx);
    __HAL_LINKDMA(uart, hdmarx, dma_rx);

    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

Uart uart2;