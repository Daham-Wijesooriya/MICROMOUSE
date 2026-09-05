/**
 ******************************************************************************
 * @file    motor_pwm.h
 * @brief   Register-level TIM1/TIM8 3-channel center-aligned PWM driver.
 *
 * Same rationale as hw_spi.h: the vendor stm32f4xx_hal_tim.{c,h} files are
 * not present in Drivers/, so this talks to TIM1 (M1: PA8/9/10) and TIM8
 * (M2: PC6/7/8) directly via CMSIS registers. GPIO alternate-function setup
 * for those pins is already done in MX_GPIO_Init() (main.c) -- this file
 * only configures the timer peripherals themselves.
 *
 * Both TIM1 and TIM8 are "advanced-control" timers on the F405: their CCx
 * outputs stay gated off at the pin regardless of CCER until BDTR.MOE=1.
 * PWM_Disable() clears MOE, which forces outputs off immediately -- this is
 * the fast-path emergency stop used by motor_test.c.
 *
 * Only CH1/2/3 (non-complementary) are used: per F405_CubeMX_pinmap.md, the
 * DRV8316s are wired for "3x PWM" mode with INLA/B/C tied to AVDD in
 * hardware, so the MCU only ever drives the high-side-equivalent compare
 * channels.
 *
 * !!! VERIFY BEFORE TRUSTING !!!
 * PWM_NEUTRAL_PCT (50% duty = commanded ~0 V phase output) is this file's
 * best-effort assumption about how DRV8316 "3x PWM" mode maps duty cycle to
 * phase voltage -- not a confirmed datasheet value. Same caveat class as
 * drv8316.h's register map.
 ******************************************************************************
 */
#ifndef MOTOR_PWM_H
#define MOTOR_PWM_H

#include "main.h"

#define PWM_NEUTRAL_PCT 50.0f

void PWM_InitTIM1(void); /* M1: PA8/PA9/PA10 -> CH1/CH2/CH3 */
void PWM_InitTIM8(void); /* M2: PC6/PC7/PC8 -> CH1/CH2/CH3  */

void PWM_Enable(TIM_TypeDef *tim);   /* MOE=1: outputs go live at their current CCR duty */
void PWM_Disable(TIM_TypeDef *tim);  /* MOE=0: outputs forced off immediately (e-stop)   */

/* a_pct/b_pct/c_pct clamped to [0, 100]. */
void PWM_SetDutyPercent(TIM_TypeDef *tim, float a_pct, float b_pct, float c_pct);

#endif /* MOTOR_PWM_H */
