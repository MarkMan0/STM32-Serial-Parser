/**
 * @file unittest_transport.c
 * Function bodies for PIO test framework communication
 *
 */

#include "unittest_transport.h"
#include <stm32f3xx_hal.h>



static UART_HandleTypeDef UartHandle;


void unittest_uart_begin() {
  UartHandle.Instance = USART2;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&UartHandle) != HAL_OK) {
    while (1) {
    }
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART2) {
    GPIO_InitTypeDef gpio = { 0 };

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Alternate = GPIO_AF7_USART2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pin = GPIO_PIN_15 | GPIO_PIN_2;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio);
  }
}

void unittest_uart_putchar(char c) {
  HAL_UART_Transmit(&UartHandle, (uint8_t*)(&c), 1, 1000);
}

void unittest_uart_flush() {
}


void unittest_uart_end() {
  __HAL_RCC_USART2_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
}
