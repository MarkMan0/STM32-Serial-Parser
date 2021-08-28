#include "adc.h"
#include "pin_api.h"

/**
 * @brief Called by HAL when init-ing the ADC
 *
 * @param hadc
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
  if (hadc->Instance == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();
  }
}


void ADC::init_adc() {
  constexpr ADC_InitTypeDef adc_init{ .ClockPrescaler = ADC_CLOCK_ASYNC_DIV1,
                                      .Resolution = ADC_RESOLUTION12b,
                                      .DataAlign = ADC_DATAALIGN_RIGHT,
                                      .ScanConvMode = ADC_SCAN_DISABLE,
                                      .EOCSelection = ADC_EOC_SINGLE_CONV,
                                      .LowPowerAutoWait = DISABLE,
                                      .ContinuousConvMode = ENABLE,
                                      .NbrOfConversion = 1,
                                      .DiscontinuousConvMode = DISABLE,
                                      .NbrOfDiscConversion = 0,
                                      .ExternalTrigConv = ADC_SOFTWARE_START,
                                      .ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
                                      .DMAContinuousRequests = DISABLE,
                                      .Overrun = ADC_OVR_DATA_OVERWRITTEN };


  hadc1_.Init = adc_init;
  hadc1_.Instance = ADC1;
  utils::hal_wrap(HAL_ADC_Init(&hadc1_));

  ADC_MultiModeTypeDef multimode{ .Mode = ADC_MODE_INDEPENDENT,
                                  .DMAAccessMode = ADC_DMAACCESSMODE_DISABLED,
                                  .TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_1CYCLE };

  utils::hal_wrap(HAL_ADCEx_MultiModeConfigChannel(&hadc1_, &multimode));

  HAL_ADC_Stop(&hadc1_);
  HAL_ADCEx_Calibration_Start(&hadc1_, ADC_SINGLE_ENDED);
}

void ADC::start_adc() {
  utils::hal_wrap(HAL_ADC_Start(&hadc1_));
}

void ADC::stop_adc() {
  utils::hal_wrap(HAL_ADC_Stop(&hadc1_));
}

uint32_t ADC::read() {
  return HAL_ADC_GetValue(&hadc1_);
}

float ADC::read_volt() {
  constexpr float scale = 3.3 / (1 << 12);
  return read() * scale;
}

ADC adc1;
