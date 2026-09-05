/**
 ******************************************************************************
 * @file    motor_pwm.c
 * @brief   Register-level TIM1/TIM8 PWM driver. See motor_pwm.h.
 ******************************************************************************
 */
#include "motor_pwm.h"

/* TIM1/TIM8 clock = 168 MHz (APB2 = 84 MHz, x2 multiplier since the APB2
 * prescaler != 1 -- see SystemClock_Config()). Center-aligned counts up
 * then down, so f_pwm = TIMCLK / (2 * (ARR+1)).
 * ARR=4199 -> 168e6 / (2*4200) = 20.0 kHz, matching the ~20-30 kHz note in
 * F405_CubeMX_pinmap.md. */
#define PWM_ARR 4199U

static void ConfigureAdvancedTimerPwm(TIM_TypeDef *tim)
{
  tim->CR1 = 0;
  tim->CR1 |= (1U << 5);   /* CMS[1:0] = 01: center-aligned mode 1 */

  tim->PSC = 0;
  tim->ARR = PWM_ARR;

  /* CH1/CH2: PWM mode 1 (OCxM=110b), preload enable (OCxPE). */
  tim->CCMR1 = (0x6U << 4) | (1U << 3)     /* OC1M, OC1PE */
             | (0x6U << 12) | (1U << 11);  /* OC2M, OC2PE */
  /* CH3: same, in CCMR2. */
  tim->CCMR2 = (0x6U << 4) | (1U << 3);    /* OC3M, OC3PE */

  /* Start at neutral (~0 V commanded phase output) before anything is enabled. */
  tim->CCR1 = (uint32_t)(PWM_ARR * (PWM_NEUTRAL_PCT / 100.0f));
  tim->CCR2 = tim->CCR1;
  tim->CCR3 = tim->CCR1;

  tim->CCER = (1U << 0) | (1U << 4) | (1U << 8); /* CC1E, CC2E, CC3E, active-high */

  tim->CR1 |= (1U << 7);   /* ARPE: auto-reload preload enable */
  tim->EGR |= (1U << 0);   /* UG: force an update so preload regs load now */

  tim->BDTR = 0;           /* MOE=0: outputs stay gated off until PWM_Enable() */

  tim->CR1 |= (1U << 0);   /* CEN: start the counter (still silent -- MOE=0) */
}

void PWM_InitTIM1(void)
{
  __HAL_RCC_TIM1_CLK_ENABLE();
  ConfigureAdvancedTimerPwm(TIM1);
}

void PWM_InitTIM8(void)
{
  __HAL_RCC_TIM8_CLK_ENABLE();
  ConfigureAdvancedTimerPwm(TIM8);
}

void PWM_Enable(TIM_TypeDef *tim)
{
  tim->BDTR |= (1U << 15); /* MOE = 1 */
}

void PWM_Disable(TIM_TypeDef *tim)
{
  tim->BDTR &= ~(1U << 15); /* MOE = 0: outputs forced to idle immediately */
}

static uint32_t DutyToCcr(float pct)
{
  if (pct < 0.0f)   { pct = 0.0f; }
  if (pct > 100.0f) { pct = 100.0f; }
  return (uint32_t)(((float)PWM_ARR) * (pct / 100.0f));
}

void PWM_SetDutyPercent(TIM_TypeDef *tim, float a_pct, float b_pct, float c_pct)
{
  tim->CCR1 = DutyToCcr(a_pct);
  tim->CCR2 = DutyToCcr(b_pct);
  tim->CCR3 = DutyToCcr(c_pct);
}
