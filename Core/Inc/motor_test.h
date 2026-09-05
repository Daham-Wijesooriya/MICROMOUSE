/**
 ******************************************************************************
 * @file    motor_test.h
 * @brief   Gated, open-loop, single-motor spin test (bring-up plan step 6g).
 *
 * This is NOT closed-loop FOC: no Clarke/Park transforms, no current
 * control, no encoder feedback. It forces a slow rotating low-amplitude
 * sinusoidal voltage vector open-loop -- exactly the "spin one motor at low
 * duty and watch for smoke/current/fault" check the bring-up plan called
 * for, using the same 3-PWM-channel interface real FOC would eventually use.
 *
 * Safety mechanism (all of this is required before an output goes live):
 *   - Bringup_RunAll() must have already run this boot and not hard-failed
 *     (a hard fail halts before this function is ever reached).
 *   - Per-motor gate: refuses to arm a motor whose DRV8316 BUCK_DIS
 *     readback did not confirm during bring-up (g_bringup.drv1/2_buck_cfg_ok)
 *     -- see drv8316.h's register-map verification warning.
 *   - Hold-to-arm: BTN_START (M1) or BTN_MODE (M2) must be held 2 s.
 *   - Visible countdown (fast red blink) before DRVOFF goes low, with an aim
 *     of giving a human time to hit an emergency stop.
 *   - Live nFAULT polled every control tick; both buttons pressed together
 *     is a manual abort. Either immediately kills PWM (MOE=0) and re-parks
 *     DRVOFF high.
 *   - Fixed low modulation depth, fixed short duration, auto ramp-down --
 *     never open-ended.
 *   - Only ever one motor at a time.
 *
 * This firmware cannot see smoke, smell burning insulation, or watch a
 * bench PSU's current display -- YOU still have to do that. Wheels must be
 * off the ground / the mouse clamped, and the bench supply current-limited,
 * before you ever hold a button here. See the bring-up plan step 6g.
 ******************************************************************************
 */
#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include "main.h"
#include <stdbool.h>

typedef struct
{
  bool m1_attempted;
  bool m1_completed_without_abort;
  bool m2_attempted;
  bool m2_completed_without_abort;
} MotorTestResults;

extern volatile MotorTestResults g_motor_test;

/* Blocks forever, watching BTN_START/BTN_MODE for arm gestures as
 * documented above. Never returns. */
void MotorTest_RunGated(void);

#endif /* MOTOR_TEST_H */
