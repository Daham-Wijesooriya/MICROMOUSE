/**
 ******************************************************************************
 * @file    bringup.h
 * @brief   Peripheral-by-peripheral bring-up test sequencer (plan steps
 *          6a-6f). No UART exists on this board, so results are:
 *            1) latched into g_bringup (inspect live with a debugger), and
 *            2) summarized on the RGB LED (see Bringup_RunAll doc comment).
 *
 *          Motor PWM (step 6g) is deliberately NOT included here -- that
 *          step requires a human watching for smoke/smell/current spikes in
 *          real time and must not be run unattended.
 ******************************************************************************
 */
#ifndef BRINGUP_H
#define BRINGUP_H

#include "main.h"
#include <stdbool.h>

typedef struct
{
  /* 6a: buttons (informational -- confirms a press was observed, not a hard fail) */
  bool btn_start_seen_pressed;
  bool btn_mode_seen_pressed;

  /* 6b: NSLEEP/DRVOFF init state */
  bool nsleep_high_ok;
  bool drvoff_high_ok;

  /* 6c: SPI2 + DRV8316 x2 */
  bool     drv1_comms_ok;      /* readback SPI frame non-trivial (not stuck 0x000/0x7FF) */
  bool     drv1_buck_cfg_ok;   /* CTRL2 readback matched what was written -- see drv8316.h warning */
  bool     drv2_comms_ok;
  bool     drv2_buck_cfg_ok;
  bool     nfault_high_after_cfg;

  /* 6d: SPI1 + AS5047P x2 */
  bool     enc1_parity_ok;
  uint16_t enc1_angle_raw;
  bool     enc2_parity_ok;
  uint16_t enc2_angle_raw;

  /* 6e: ICM-42670 WHO_AM_I */
  uint8_t imu_whoami_raw;
  bool    imu_whoami_match;    /* see icm42670.h "verify before trusting" note */

  /* 6f: IR raw + emitter pulse */
  uint16_t ir_baseline[6];     /* emitters off */
  uint16_t ir_lit[6];          /* during this channel's own emitter pulse */
  bool     ir_responds[6];     /* ir_lit - ir_baseline exceeded threshold */

  bool any_hard_fail;          /* set just before Bringup_Halt() is called   */
  bool any_soft_issue;         /* set for anything that continues but is off */
} BringupResults;

extern volatile BringupResults g_bringup;

void Bringup_RunAll(void);

#endif /* BRINGUP_H */
