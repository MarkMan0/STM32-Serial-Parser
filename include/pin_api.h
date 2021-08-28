#ifndef PIN_API_H_
#define PIN_API_H_

/**
 * @file pin_api.h
 * @brief Lightweight wrapper around HAL GPIO API
 *
 */

#include <stm32f3xx_hal.h>
#include "utils.h"
#include "adc.h"

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
   * @param init GPIO_InitTypeDef for this GPIO
   */
  constexpr Pin_t(uint32_t base, const GPIO_InitTypeDef& init) : base_{ base }, init_{ init } {};

  /**
   * @brief Inits the pin using HAL_GPIO_Init
   *
   * Even though this method can be overriden in the other Pin classes,
   * it's not marked virtual, because it's not expected,
   * that the init method will be called via a base class pointer/reference
   *
   * IMPORTANT: method is NOT virtual, so should not be called via base class
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

  /**
   * @brief Toggles the pin
   *
   */
  void toggle() const {
    HAL_GPIO_TogglePin(get_ptr(), init_.Pin);
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

/**
 * @brief Pin that will be used for ADC reading
 *
 */
class ADCPin_t : public Pin_t {
public:
  /**
   * @brief Constructs an ADCPin_t
   * @see Pin_t
   * @param adc pointer to the ADC_HandleTypeDef for this pin
   * @param adc_init ADC_ChannelConfTypeDef for this pin, rank can be changed later
   */
  constexpr ADCPin_t(uint32_t base, const GPIO_InitTypeDef& init, ADC_HandleTypeDef* adc,
                     const ADC_ChannelConfTypeDef& adc_init)
    : Pin_t(base, init), hadc_{ adc }, adc_init_{ adc_init } {
  }

  /**
   * @brief Applies the rank to the channel and configs it, using HAL_ADC_ConfigChannel
   *
   * @param rank the rank of the channel
   */
  void config_channel(uint32_t rank) const {
    ADC_ChannelConfTypeDef ch_conf{ adc_init_ };
    ch_conf.Rank = rank;
    utils::hal_wrap(HAL_ADC_ConfigChannel(hadc_, (&ch_conf)));
  }

  /**
   * @brief Config the channel with the rank given in the constructor
   *
   */
  void config_channel() const {
    config_channel(adc_init_.Rank);
  }

private:
  ADC_HandleTypeDef* hadc_;               //!< ADC handle for this pin */
  const ADC_ChannelConfTypeDef adc_init_; /*!< The default channel config */
};

/**
 * @brief Pin definitions
 *
 */
namespace pins {
  // clang-format off
  constexpr Pin_t
      led{ GPIOB_BASE,
          GPIO_InitTypeDef{ GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },

      uart2_rx{ GPIOA_BASE,
              GPIO_InitTypeDef{ GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2 } },

      uart2_tx{ GPIOA_BASE,
              GPIO_InitTypeDef{ GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2 } },

      A1{ GPIOA_BASE,
          GPIO_InitTypeDef{ GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } };

  constexpr ADCPin_t
      A0{ GPIOA_BASE,
          GPIO_InitTypeDef{ GPIO_PIN_0, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 },
          &adc1.hadc1_,
          ADC_ChannelConfTypeDef{ADC_CHANNEL_1, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_1CYCLE_5, ADC_SINGLE_ENDED, ADC_OFFSET_NONE, 0} };

  // clang-format on
}  // namespace pins

#endif
