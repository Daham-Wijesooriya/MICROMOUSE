/**
 ******************************************************************************
 * @file    motor_test.c
 * @brief   Gated, open-loop, single-motor spin test. See motor_test.h.
 ******************************************************************************
 */
#include "motor_test.h"
#include "bringup.h"
#include "motor_pwm.h"

volatile MotorTestResults g_motor_test;

#define ARM_HOLD_MS       2000U  /* must hold the button this long to arm       */
#define COUNTDOWN_MS      1500U  /* warning blink before outputs go live        */
#define RAMP_MS           1500U  /* 0 -> MAX_MOD_PCT and back                   */
#define HOLD_MS           3000U  /* time spent at MAX_MOD_PCT                   */
#define MAX_MOD_PCT       8.0f   /* modulation depth -- LOW, per bring-up plan  */
#define CONTROL_PERIOD_MS 5U     /* abort-check / duty-update tick              */
#define STEP_MS           42U    /* ~2 Hz electrical open-loop rotation (very slow, deliberate) */

/* 12-point sine table, 30 degree steps: sin(0),sin(30),...,sin(330).
 * Phase B / phase C are the same table read 4 / 8 steps ahead (120 deg /
 * 240 deg, exact since 360/30/3 = 4). No libm dependency. */
static const float kSin12[12] =
{
   0.0f,  0.5f,  0.8660254f,  1.0f,  0.8660254f,  0.5f,
   0.0f, -0.5f, -0.8660254f, -1.0f, -0.8660254f, -0.5f,
};

static void LedSet(bool r, bool g, bool b)
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* Blocks until the button has been held continuously for ms, or returns
 * false the moment it's released early. */
static bool ButtonHeld(GPIO_TypeDef *port, uint16_t pin, uint32_t ms)
{
  uint32_t start = HAL_GetTick();
  while (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) /* pressed = LOW (pull-up) */
  {
    if ((HAL_GetTick() - start) >= ms) { return true; }
  }
  return false;
}

/* Checked every control tick while outputs are live. Real fault, or a
 * deliberate two-button chord, both mean "stop now". */
static bool AbortRequested(void)
{
  if (HAL_GPIO_ReadPin(NFAULT_GPIO_Port, NFAULT_Pin) == GPIO_PIN_RESET) { return true; }

  bool m1_btn = (HAL_GPIO_ReadPin(BTN_START_GPIO_Port, BTN_START_Pin) == GPIO_PIN_RESET);
  bool m2_btn = (HAL_GPIO_ReadPin(BTN_MODE_GPIO_Port, BTN_MODE_Pin) == GPIO_PIN_RESET);
  if (m1_btn && m2_btn) { return true; } /* manual abort chord */

  return false;
}

static void EmergencyStop(TIM_TypeDef *tim)
{
  PWM_Disable(tim); /* outputs off first -- immediate, independent of DRVOFF */
  PWM_SetDutyPercent(tim, PWM_NEUTRAL_PCT, PWM_NEUTRAL_PCT, PWM_NEUTRAL_PCT);
  HAL_GPIO_WritePin(DRVOFF_GPIO_Port, DRVOFF_Pin, GPIO_PIN_SET); /* back to safe/Hi-Z */
}

/* Ramps modulation 0 -> MAX_MOD_PCT -> 0 while forcing a slow rotating
 * 3-phase voltage vector, open loop (no encoder, no current feedback).
 * Returns false if aborted early (fault or manual chord); output is left
 * safe (PWM disabled, DRVOFF high) either way. */
static bool RunOpenLoopSpin(TIM_TypeDef *tim)
{
  /* Duty already sits at neutral from init/last EmergencyStop -- confirm
   * before unmasking outputs, then enable PWM, then enable the driver. */
  PWM_SetDutyPercent(tim, PWM_NEUTRAL_PCT, PWM_NEUTRAL_PCT, PWM_NEUTRAL_PCT);
  PWM_Enable(tim);
  HAL_GPIO_WritePin(DRVOFF_GPIO_Port, DRVOFF_Pin, GPIO_PIN_RESET); /* driver outputs live, ~0 V commanded */

  const uint32_t total_ms = RAMP_MS + HOLD_MS + RAMP_MS;
  uint32_t phase_start = HAL_GetTick();
  uint32_t elapsed;

  while ((elapsed = (HAL_GetTick() - phase_start)) < total_ms)
  {
    if (AbortRequested())
    {
      EmergencyStop(tim);
      return false;
    }

    float mod;
    if (elapsed < RAMP_MS)
    {
      mod = MAX_MOD_PCT * ((float)elapsed / (float)RAMP_MS);
    }
    else if (elapsed < (RAMP_MS + HOLD_MS))
    {
      mod = MAX_MOD_PCT;
    }
    else
    {
      uint32_t down_elapsed = elapsed - RAMP_MS - HOLD_MS;
      mod = MAX_MOD_PCT * (1.0f - ((float)down_elapsed / (float)RAMP_MS));
    }

    uint32_t idx   = (elapsed / STEP_MS) % 12U;
    uint32_t idx_b = (idx + 4U) % 12U;
    uint32_t idx_c = (idx + 8U) % 12U;

    PWM_SetDutyPercent(tim,
      PWM_NEUTRAL_PCT + mod * kSin12[idx],
      PWM_NEUTRAL_PCT + mod * kSin12[idx_b],
      PWM_NEUTRAL_PCT + mod * kSin12[idx_c]);

    HAL_Delay(CONTROL_PERIOD_MS);
  }

  EmergencyStop(tim); /* duty already ramped back near neutral; this re-parks DRVOFF high */
  return true;
}

/* Hold-to-arm -> countdown (abortable) -> run. Refuses outright if this
 * driver's BUCK_DIS write was never confirmed during bring-up. */
static void TryArmAndRun(GPIO_TypeDef *arm_port, uint16_t arm_pin, TIM_TypeDef *tim,
                          bool buck_cfg_ok, volatile bool *attempted, volatile bool *completed_ok)
{
  if (!buck_cfg_ok) { return; } /* BUCK_DIS readback never confirmed -- refuse to arm */
  if (!ButtonHeld(arm_port, arm_pin, ARM_HOLD_MS)) { return; }

  *attempted = true;

  bool proceed = true;
  uint32_t t0 = HAL_GetTick();
  bool on = false;
  while ((HAL_GetTick() - t0) < COUNTDOWN_MS)
  {
    on = !on;
    LedSet(on, false, false); /* fast red blink: outputs about to go live */
    HAL_Delay(100);
    if (AbortRequested()) { proceed = false; break; }
  }

  if (proceed)
  {
    *completed_ok = RunOpenLoopSpin(tim);
  }
}

void MotorTest_RunGated(void)
{
  PWM_InitTIM1();
  PWM_InitTIM8();

  while (1)
  {
    LedSet(false, true, false); /* idle: solid green, ready and waiting */

    bool m1_btn = (HAL_GPIO_ReadPin(BTN_START_GPIO_Port, BTN_START_Pin) == GPIO_PIN_RESET);
    bool m2_btn = (HAL_GPIO_ReadPin(BTN_MODE_GPIO_Port, BTN_MODE_Pin) == GPIO_PIN_RESET);

    if (m1_btn && !m2_btn)
    {
      TryArmAndRun(BTN_START_GPIO_Port, BTN_START_Pin, TIM1,
                   g_bringup.drv1_buck_cfg_ok,
                   &g_motor_test.m1_attempted, &g_motor_test.m1_completed_without_abort);
    }
    else if (m2_btn && !m1_btn)
    {
      TryArmAndRun(BTN_MODE_GPIO_Port, BTN_MODE_Pin, TIM8,
                   g_bringup.drv2_buck_cfg_ok,
                   &g_motor_test.m2_attempted, &g_motor_test.m2_completed_without_abort);
    }

    HAL_Delay(20);
  }
}
