/**
 ******************************************************************************
 * @file    ir_sense.c
 * @brief   IR_RX raw ADC reads + IR_EM gate-drive pulses. See ir_sense.h.
 ******************************************************************************
 */
#include "ir_sense.h"
#include "adc_bus.h"

typedef struct
{
  ADC_HandleTypeDef *hadc;
  uint32_t            channel;
} IrRxMap;

/* Order: RX1, RX2, RX3, RX4, RX5, RX6 -- per F405_CubeMX_pinmap.md. */
static const IrRxMap kIrRxMap[IR_CHANNEL_COUNT] =
{
  { &hadc1, ADC_CHANNEL_13 }, /* RX1 PC3 */
  { &hadc2, ADC_CHANNEL_14 }, /* RX2 PC4 */
  { &hadc2, ADC_CHANNEL_15 }, /* RX3 PC5 */
  { &hadc1, ADC_CHANNEL_8  }, /* RX4 PB0 */
  { &hadc1, ADC_CHANNEL_9  }, /* RX5 PB1 */
  { &hadc1, ADC_CHANNEL_3  }, /* RX6 PA3 */
};

/* Order: EM1..EM6 -- per F405_CubeMX_pinmap.md. */
static GPIO_TypeDef *const kEmPort[IR_CHANNEL_COUNT] =
{
  IR_EM1_GPIO_Port, IR_EM2_GPIO_Port, IR_EM3_GPIO_Port,
  IR_EM4_GPIO_Port, IR_EM5_GPIO_Port, IR_EM6_GPIO_Port,
};
static const uint16_t kEmPin[IR_CHANNEL_COUNT] =
{
  IR_EM1_Pin, IR_EM2_Pin, IR_EM3_Pin, IR_EM4_Pin, IR_EM5_Pin, IR_EM6_Pin,
};

static void DwtDelayInit(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void DelayUs(uint32_t us)
{
  uint32_t cycles = us * (SystemCoreClock / 1000000U);
  uint32_t start = DWT->CYCCNT;
  while ((DWT->CYCCNT - start) < cycles) { }
}

void IR_Init(void)
{
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef gi = {0};
  gi.Pin   = IR_EM6_Pin;
  gi.Mode  = GPIO_MODE_OUTPUT_PP;
  gi.Pull  = GPIO_NOPULL;
  gi.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IR_EM6_GPIO_Port, &gi);
  HAL_GPIO_WritePin(IR_EM6_GPIO_Port, IR_EM6_Pin, GPIO_PIN_RESET);

  DwtDelayInit();
}

static uint16_t ReadOne(ADC_HandleTypeDef *hadc, uint32_t channel)
{
  ADC_ChannelConfTypeDef cfg = {0};
  cfg.Channel = channel;
  cfg.Rank = 1;
  cfg.SamplingTime = ADC_SAMPLETIME_84CYCLES; /* slow/settled reading for bring-up */
  HAL_ADC_ConfigChannel(hadc, &cfg);

  HAL_ADC_Start(hadc);
  HAL_ADC_PollForConversion(hadc, 10);
  uint16_t val = (uint16_t)HAL_ADC_GetValue(hadc);
  HAL_ADC_Stop(hadc);
  return val;
}

void IR_ReadAllRaw(uint16_t out[IR_CHANNEL_COUNT])
{
  for (int i = 0; i < IR_CHANNEL_COUNT; i++)
  {
    out[i] = ReadOne(kIrRxMap[i].hadc, kIrRxMap[i].channel);
  }
}

void IR_SetEmitter(uint8_t idx, bool on)
{
  if (idx >= IR_CHANNEL_COUNT) { return; }
  HAL_GPIO_WritePin(kEmPort[idx], kEmPin[idx], on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void IR_PulseEmitter(uint8_t idx, uint32_t pulse_us)
{
  if (idx >= IR_CHANNEL_COUNT) { return; }
  HAL_GPIO_WritePin(kEmPort[idx], kEmPin[idx], GPIO_PIN_SET);
  DelayUs(pulse_us);
  HAL_GPIO_WritePin(kEmPort[idx], kEmPin[idx], GPIO_PIN_RESET);
}
