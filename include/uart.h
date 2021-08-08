#ifndef UART_H_
#define UART_H_

#include "main.h"
#include <cstring>

class Uart {
public:
  /**
   * @brief handle to uart
   *
   */
  UART_HandleTypeDef huart;

  /**
   * @brief initializes the UART peripherial
   *
   */
  void init();

  /**
   * @brief Non blocking transmission via UART.
   *
   * @param data pointer to data to be transmitted
   * @param len number of bytes to be transmitted
   * @return HAL_StatusTypeDef HAL_OK on success
   */
  HAL_StatusTypeDef transmit(uint8_t* data, size_t len);
  /**
   * @brief Non blocking, Used to transmit char[] arrays what are declared by the user.
   *
   * @tparam N automatically deduces by the compiler
   * @return HAL_StatusTypeDef HAL_OK on succes
   * @see transmit(uint8_t* data, size_t len)
   */
  template <size_t N>
  HAL_StatusTypeDef transmit(const char (&arr)[N]);
  /**
   * @brief Non blocking transmission of a null-terminated array
   *
   * @param data pointer to null terminated char array
   * @return HAL_StatusTypeDef HAL_OK on success
   */
  HAL_StatusTypeDef transmit(char* data);

  /**
   * @brief Transmits data using DMA
   *
   * @param data pointer to data
   * @param len length of data
   * @return HAL_StatusTypeDef
   * @see transmit(uint8_t* data, size_t len)
   */
  HAL_StatusTypeDef transmit_DMA(uint8_t* data, size_t len);
  /**
   * @brief transmit a char[] array using DMA
   *
   * @tparam N lenght of array, deduced by the compiler
   * @return HAL_StatusTypeDef
   * @see transmit(const char (&arr)[N])
   */
  template <size_t N>
  HAL_StatusTypeDef transmit_DMA(const char (&arr)[N]);
  /**
   * @brief Transmits a null-terminated array using DMA
   *
   * @param data pointer to null-terminated array
   * @return HAL_StatusTypeDef
   * @see transmit(char* data)
   */
  HAL_StatusTypeDef transmit_DMA(char* data);
};

inline HAL_StatusTypeDef Uart::transmit(uint8_t* data, size_t len) {
  return HAL_UART_Transmit(&huart, data, len, 500);
}

template <size_t N>
inline HAL_StatusTypeDef Uart::transmit(const char (&arr)[N]) {
  if (N == 0)
    return HAL_OK;
  return transmit(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
}

inline HAL_StatusTypeDef Uart::transmit(char* data) {
  return transmit(reinterpret_cast<uint8_t*>(data), strlen(data));
}

inline HAL_StatusTypeDef Uart::transmit_DMA(uint8_t* data, size_t len) {
  const auto start = HAL_GetTick();
  while (HAL_UART_Transmit_DMA(&huart, data, len) != HAL_OK) {
    if (HAL_GetTick() > (start + 100)) {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

template <size_t N>
inline HAL_StatusTypeDef Uart::transmit_DMA(const char (&arr)[N]) {
  if (N == 0)
    return HAL_OK;
  return transmit_DMA(reinterpret_cast<uint8_t*>(const_cast<char*>(arr)), N - 1);
}

inline HAL_StatusTypeDef Uart::transmit_DMA(char* data) {
  return transmit_DMA(reinterpret_cast<uint8_t*>(data), strlen(data));
}

extern Uart uart2;

#endif  // UART_H_