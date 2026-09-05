/**
 ******************************************************************************
 * @file    bringup.c
 * @brief   Bring-up test sequencer. See bringup.h.
 *
 * LED status codes (LED_RED/LED_GRN/LED_BLUE, PC10/11/12):
 *   - Solid BLUE, blinking during 6a : waiting for BTN_START/BTN_MODE presses
 *   - Solid RED (steady, forever)    : hard fail -- halted, inspect g_bringup
 *   - Alternating RED+GRN ("amber")  : all stages ran, at least one soft
 *                                      issue flagged -- inspect g_bringup
 *   - Solid GREEN (steady, forever)  : all stages passed clean
 ******************************************************************************
 */
#include "bringup.h"
#include "hw_spi.h"
#include "drv8316.h"
#include "as5047p.h"
#include "icm42670.h"
#include "ir_sense.h"

volatile BringupResults g_bringup;

/* IR channel response threshold: raw 12-bit ADC counts the lit reading must
 * exceed the dark baseline by. Arbitrary/tunable -- retune once you know
 * your phototransistor's real dark/lit spread on this board. */
#define IR_RESPONSE_DELTA_THRESHOLD 50U

static void LedSet(bool r, bool g, bool b)
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* Stop right here on any hard fail. Halts with interrupts off and a
 * breakpoint instruction so a debugger drops straight in; g_bringup holds
 * every value captured so far. If no debugger is attached, the LED stays
 * solid red so the failure is still visible on the bench. */
static void Bringup_Halt(void)
{
  g_bringup.any_hard_fail = true;
  LedSet(true, false, false);
  __disable_irq();
  __BKPT(0);
  while (1) { }
}

static void Stage_Buttons(void)
{
  uint32_t start = HAL_GetTick();
  bool led_on = false;
  uint32_t last_toggle = start;

  while ((HAL_GetTick() - start) < 3000U)
  {
    if (HAL_GPIO_ReadPin(BTN_START_GPIO_Port, BTN_START_Pin) == GPIO_PIN_RESET)
    {
      g_bringup.btn_start_seen_pressed = true;
    }
    if (HAL_GPIO_ReadPin(BTN_MODE_GPIO_Port, BTN_MODE_Pin) == GPIO_PIN_RESET)
    {
      g_bringup.btn_mode_seen_pressed = true;
    }

    if ((HAL_GetTick() - last_toggle) > 150U)
    {
      led_on = !led_on;
      LedSet(false, false, led_on);
      last_toggle = HAL_GetTick();
    }
  }
  LedSet(false, false, false);

  if (!g_bringup.btn_start_seen_pressed || !g_bringup.btn_mode_seen_pressed)
  {
    /* Informational only -- a human may simply not have pressed them yet. */
    g_bringup.any_soft_issue = true;
  }
}

static void Stage_NsleepDrvoff(void)
{
  g_bringup.nsleep_high_ok = (HAL_GPIO_ReadPin(NSLEEP_GPIO_Port, NSLEEP_Pin) == GPIO_PIN_SET);
  g_bringup.drvoff_high_ok = (HAL_GPIO_ReadPin(DRVOFF_GPIO_Port, DRVOFF_Pin) == GPIO_PIN_SET);

  if (!g_bringup.nsleep_high_ok)
  {
    /* Driver never woke -- every SPI2 read below will fail anyway. */
    Bringup_Halt();
  }
  if (!g_bringup.drvoff_high_ok)
  {
    /* Regression of the step-0 firmware fix: DRVOFF must stay high until
     * BUCK_DIS is confirmed written. Do NOT continue toward enabling
     * outputs on an unverified buck network. */
    Bringup_Halt();
  }
}

static void Stage_Spi2Drivers(void)
{
  DRV8316_Handle drv1 = { DRV1_CS_GPIO_Port, DRV1_CS_Pin };
  DRV8316_Handle drv2 = { DRV2_CS_GPIO_Port, DRV2_CS_Pin };

  uint8_t r1 = DRV8316_ReadReg(&drv1, DRV8316_REG_IC_STATUS);
  uint8_t r2 = DRV8316_ReadReg(&drv2, DRV8316_REG_IC_STATUS);

  /* Heuristic only: MISO stuck all-0 or all-1 means "bus is dead", not a
   * real register value. Data field is 8 bits now that the frame format is
   * corrected (R/W:1 addr:6 parity:1 data:8) -- see drv8316.h. */
  g_bringup.drv1_comms_ok = (r1 != 0x00U) && (r1 != 0xFFU);
  g_bringup.drv2_comms_ok = (r2 != 0x00U) && (r2 != 0xFFU);

  if (!g_bringup.drv1_comms_ok && !g_bringup.drv2_comms_ok)
  {
    /* Both drivers dead on the shared bus. The SPI2_MISO 10k pull-up bodge
     * is now confirmed physically installed (F405_CubeMX_pinmap.md), so
     * this is no longer the prime suspect it once was -- if this still
     * fires, look at CS wiring, VM reaching both drivers, or a real fault
     * on the shared bus rather than assuming the pull-up again. */
    Bringup_Halt();
  }

  /* Attempt to clear any latched fault before configuring -- some gate
   * drivers refuse config writes while a fault is latched, which would
   * otherwise show up here as "buck config failed" for the wrong reason. */
  DRV8316_ClearFaults(&drv1);
  DRV8316_ClearFaults(&drv2);

  g_bringup.drv1_buck_cfg_ok = DRV8316_ConfigureBuckDisablePwmMode(&drv1);
  g_bringup.drv2_buck_cfg_ok = DRV8316_ConfigureBuckDisablePwmMode(&drv2);

  if (!g_bringup.drv1_buck_cfg_ok || !g_bringup.drv2_buck_cfg_ok)
  {
    /* Not a hard fail by itself: the register map is now cross-checked
     * against a working reference driver (see drv8316.h) but still not the
     * TI datasheet directly. Flag and keep going so you can still check
     * nFAULT below. */
    g_bringup.any_soft_issue = true;
  }

  g_bringup.nfault_high_after_cfg = (HAL_GPIO_ReadPin(NFAULT_GPIO_Port, NFAULT_Pin) == GPIO_PIN_SET);
  if (!g_bringup.nfault_high_after_cfg)
  {
    /* Real fault asserted after touching the driver config -- stop.
     * nFAULT's 10k pull-up bodge is now confirmed installed
     * (F405_CubeMX_pinmap.md), so a low reading here means something
     * actually pulled it low, not a floating pin. Per that doc, DRV8316
     * requires nFAULT >2.2V at power-up or it enters test mode; since 3V3
     * (from the buck) can lag VM slightly on power-up, a fault latched from
     * that brief window at boot is possible -- DRV8316_ClearFaults() above
     * is the attempt to recover from that before concluding it's a live
     * fault. */
    Bringup_Halt();
  }
}

static void Stage_Spi1Encoders(void)
{
  AS5047P_Reading e1 = AS5047P_ReadAngle(ENC1_CS_GPIO_Port, ENC1_CS_Pin);
  AS5047P_Reading e2 = AS5047P_ReadAngle(ENC2_CS_GPIO_Port, ENC2_CS_Pin);

  g_bringup.enc1_parity_ok = (e1.parity_ok != 0U) && (e1.error_flag == 0U);
  g_bringup.enc1_angle_raw = e1.angle_raw;
  g_bringup.enc2_parity_ok = (e2.parity_ok != 0U) && (e2.error_flag == 0U);
  g_bringup.enc2_angle_raw = e2.angle_raw;

  if (!g_bringup.enc1_parity_ok || !g_bringup.enc2_parity_ok)
  {
    g_bringup.any_soft_issue = true;
  }
  /* No motor spin here -- to confirm the angle actually TRACKS rotation,
   * rotate each wheel by hand while watching enc1_angle_raw/enc2_angle_raw
   * live in the debugger. */
}

static void Stage_Imu(void)
{
  g_bringup.imu_whoami_raw = ICM42670_ReadWhoAmI();
  g_bringup.imu_whoami_match = (g_bringup.imu_whoami_raw == ICM42670_WHOAMI_EXPECTED);

  if (!g_bringup.imu_whoami_match)
  {
    /* Expected ID itself is an unverified placeholder -- see icm42670.h. */
    g_bringup.any_soft_issue = true;
  }
}

static void Stage_IrSensors(void)
{
  uint16_t baseline[6];
  IR_ReadAllRaw(baseline);
  for (uint8_t i = 0; i < 6; i++) { g_bringup.ir_baseline[i] = baseline[i]; }

  for (uint8_t i = 0; i < 6; i++)
  {
    IR_SetEmitter(i, true);
    HAL_Delay(1); /* settle */
    uint16_t all[6];
    IR_ReadAllRaw(all);
    IR_SetEmitter(i, false);

    g_bringup.ir_lit[i] = all[i];

    int32_t delta = (int32_t)all[i] - (int32_t)baseline[i];
    g_bringup.ir_responds[i] = (delta > (int32_t)IR_RESPONSE_DELTA_THRESHOLD);

    if (!g_bringup.ir_responds[i])
    {
      g_bringup.any_soft_issue = true;
    }
  }
}

void Bringup_RunAll(void)
{
  /* Order matters here: everything except Stage_Spi2Drivers runs entirely
   * off logic 3V3 (buttons/LEDs are plain GPIO; the AS5047Ps, ICM-42670,
   * and IR emitters/receivers are all 3V3-powered per the schematic) --
   * none of them need VM/battery. Stage_Spi2Drivers is VM-dependent (the
   * DRV8316s generate their own digital supply from VM) and WILL hard-halt
   * with no VM present, which is correct behavior, not a bug -- so it runs
   * LAST, to avoid blocking bench-only (ST-Link power, no VM) sessions from
   * getting through the encoder/IMU/IR checks first. */
  Stage_Buttons();          /* 6a */
  Stage_NsleepDrvoff();     /* 6b -- halts here if wrong (MCU-side only, no VM needed) */
  Stage_Spi1Encoders();     /* 6d */
  Stage_Imu();              /* 6e */
  Stage_IrSensors();        /* 6f */
  Stage_Spi2Drivers();      /* 6c -- halts here on dead bus or real fault; needs VM */

  /* Reached the end without a hard fail (Bringup_Halt() never returns).
   * Show the summary color briefly, then hand control back to main() --
   * whatever runs next (e.g. MotorTest_RunGated()) checks the specific
   * g_bringup flags it cares about rather than this one-bit summary. */
  if (g_bringup.any_soft_issue)
  {
    for (int i = 0; i < 4; i++)
    {
      LedSet(true, false, false);
      HAL_Delay(300);
      LedSet(false, true, false);
      HAL_Delay(300);
    }
  }
  else
  {
    LedSet(false, true, false);
    HAL_Delay(1000);
  }
}
