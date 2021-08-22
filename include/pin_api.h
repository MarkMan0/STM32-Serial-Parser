#ifndef PIN_API_H_
#define PIN_API_H_

/**
 * @file pin_api.h
 * @brief Lightweight wrapper around HAL GPIO API
 *
 */

#include <stm32f3xx_hal.h>


extern "C" void Error_Handler(void);

/**
 * @brief Creates a pin definition
 *
 * Wrapper around HAL GPIO api
 */
class Pin_t {
public:
  /**
   * @brief Constructor, creates an internal GPIO_InitTypeDef
   * @param base Address of GPIO Base (GPIOA_BASE, GPIOB_BASE)
   * @param pin Pin number from hal (GPIO_PIN_0)
   * @param mode Pin mode from hal (GPIO_MODE_OUTPUT_PP)
   * @param pull Pullup, pulldown config (GPIO_NOPULL)
   * @param speed GPIO speed GPIO_SPEED_FREQ_LOW
   * @param alternate Pin alternate function
   */
  constexpr Pin_t(uint32_t base, uint32_t pin, uint32_t mode = GPIO_MODE_INPUT, uint32_t pull = GPIO_NOPULL,
                  uint32_t speed = GPIO_SPEED_FREQ_LOW, uint32_t alternate = 0)
    : base_{ base }, init_{ .Pin = pin, .Mode = mode, .Pull = pull, .Speed = speed, .Alternate = alternate } {};

  /**
   * @brief Inits the pin using HAL_GPIO_Init
   *
   */
  void init() const {
    auto cpy = init_;
    enable_clk();
    HAL_GPIO_Init(get_ptr(), &cpy);
  }

  /**
   * @brief Writes the state to the pin
   *
   * @param state
   */
  void write(GPIO_PinState state) const {
    HAL_GPIO_WritePin(get_ptr(), init_.Pin, state);
  }

  /**
   * @brief Writes the state to the pin
   *
   * @param state
   */
  void write(bool state) const {
    write(state ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  /**
   * @brief Reads the pin state
   *
   * @return true
   * @return false
   */
  bool read() const {
    return HAL_GPIO_ReadPin(get_ptr(), init_.Pin) == GPIO_PIN_SET;
  }

private:
  const uint32_t base_;         /*!< GPIO base address */
  const GPIO_InitTypeDef init_; /*!< init struct created using constructor */

  /**
   * @brief Converts the base address into a pointer
   *
   * @return GPIO_TypeDef*
   */
  GPIO_TypeDef* get_ptr() const {
    return reinterpret_cast<GPIO_TypeDef*>(base_);
  }

  /**
   * @brief Enables the correct clock for the GPIO
   *
   */
  void enable_clk() const {
    switch (base_) {
#ifdef GPIOA_BASE
      case GPIOA_BASE:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
#endif
#ifdef GPIOB_BASE
      case GPIOB_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOC_BASE
      case GPIOC_BASE:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
#endif
#ifdef GPIOD_BASE
      case GPIOD_BASE:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;
#endif
#ifdef GPIOE_BASE
      case GPIOE_BASE:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
#endif
#ifdef GPIOF_BASE
      case GPIOF_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOG_BASE
      case GPIOG_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOH_BASE
      case GPIOH_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
      default:
        Error_Handler();
        break;
    }
  }
};
namespace pins {
  /**
   * @brief Pin definitions
   *
   */
  // clang-format off
  constexpr Pin_t
      led{ GPIOB_BASE, GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 },
      uart2_rx{ GPIOA_BASE, GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2 },
      uart2_tx{ GPIOA_BASE, GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2 };
  // clang-format on
}  // namespace pins

#endif
