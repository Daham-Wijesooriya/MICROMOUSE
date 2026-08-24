/**
 ******************************************************************************
 * @file    drv8316.h
 * @brief   DRV8316 gate driver, SPI register access over SPI2.
 *
 * !!! REGISTER MAP NOT YET VERIFIED -- READ THIS BEFORE FLASHING !!!
 *
 * The 16-bit frame shape below (R/W in bit15, 4-bit address in bits14:11,
 * 11-bit data in bits10:0) matches TI's DRV83xx SPI gate-driver family
 * convention (DRV8323/8353-style) that DRV8316 is presumed to share, but
 * this has NOT been cross-checked against the DRV8316 datasheet register
 * table. Likewise DRV8316_REG_FAULT_STATUS, DRV8316_REG_CTRL2, and the
 * BUCK_DIS / PWM_MODE bit positions below are placeholders -- fill them in
 * from the datasheet's register map (SLVSF... register section) before
 * trusting DRV_ConfigureBuckDisablePwmMode() to actually do what its name
 * says. Until verified, treat any register read/write here as "exercises
 * the SPI bus" only, not "confirmed correct configuration."
 *
 * Per F405_CubeMX_pinmap.md driver init order, this must run (and BUCK_DIS
 * must read back as written, and nFAULT must be high) BEFORE DRVOFF is ever
 * pulled low -- see bringup.c stage ordering.
 ******************************************************************************
 */
#ifndef DRV8316_H
#define DRV8316_H

#include "main.h"
#include <stdbool.h>

/* ---- TODO: verify against DRV8316 datasheet register map ---- */
#define DRV8316_REG_FAULT_STATUS   0x00U   /* placeholder address */
#define DRV8316_REG_CTRL2          0x05U   /* placeholder address */
#define DRV8316_CTRL2_BUCK_DIS_BIT 0U      /* placeholder bit position */
#define DRV8316_CTRL2_PWM_MODE_POS 1U      /* placeholder bit position, 2 bits wide */
#define DRV8316_CTRL2_PWM_MODE_3X  (2U << DRV8316_CTRL2_PWM_MODE_POS) /* "3x PWM" = 0b10 per pinmap note */
/* ---------------------------------------------------------------- */

typedef struct
{
  GPIO_TypeDef *cs_port;
  uint16_t      cs_pin;
} DRV8316_Handle;

uint16_t DRV8316_ReadReg(const DRV8316_Handle *h, uint8_t addr);
void     DRV8316_WriteReg(const DRV8316_Handle *h, uint8_t addr, uint16_t data11);

/* Writes BUCK_DIS=1 and PWM_MODE=3x to CTRL2, reads it back, returns true
 * only if the readback matches what was written. */
bool DRV8316_ConfigureBuckDisablePwmMode(const DRV8316_Handle *h);

#endif /* DRV8316_H */
