/**
 ******************************************************************************
 * @file    drv8316.h
 * @brief   DRV8316 gate driver, SPI register access over SPI2.
 *
 * Frame format and register addresses below are cross-checked against the
 * open-source DRV8316 driver in SimpleFOC's Arduino-FOC-drivers repo
 * (src/drivers/drv8316/{drv8316.cpp,drv8316_registers.h}), which implements
 * a working DRV8316 SPI client -- not just transcribed from the datasheet
 * PDF directly (that didn't extract cleanly). Treat this as "verified
 * against a known-working reference implementation," a step better than
 * the earlier placeholder guess, but if register reads still look wrong on
 * real hardware, re-check against the actual TI datasheet register table.
 *
 * 16-bit SPI frame, MSB first:
 *   bit15    = R/W (1 = read, 0 = write)
 *   bits14:9 = 6-bit register address
 *   bit8     = parity (even parity over all 16 bits, including this bit)
 *   bits7:0  = 8-bit data (write payload, or read-response register value)
 *
 * SPI mode: Mode 1 (CPOL=0, CPHA=1) -- same as hw_spi.c's SPI2 config,
 * already correct. Reference driver runs SPI2 at 1 MHz; hw_spi.c now does
 * the same (see SPI2_HW_Init()) rather than the faster, unverified 2.625 MHz
 * used before.
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

/* Register addresses (from the reference driver's drv8316_registers.h). */
#define DRV8316_REG_IC_STATUS   0x00U /* fault/status summary -- used as the "comms alive" probe */
#define DRV8316_REG_STATUS_1    0x01U
#define DRV8316_REG_STATUS_2    0x02U
#define DRV8316_REG_CONTROL_1   0x03U
#define DRV8316_REG_CONTROL_2   0x04U /* CLR_FLT bit0, PWM_MODE bits[2:1], SLEW bits[4:3], SDO_MODE bit5 */
#define DRV8316_REG_CONTROL_3   0x05U
#define DRV8316_REG_CONTROL_4   0x06U
#define DRV8316_REG_CONTROL_5   0x07U
#define DRV8316_REG_CONTROL_6   0x08U /* BUCK_DIS bit0, BUCK_SEL bits[2:1], BUCK_CL bit3, BUCK_PS_DIS bit4 */

/* Control_2 (0x04): PWM_MODE field, bits[2:1]. "3x PWM" = 0b10, matching
 * F405_CubeMX_pinmap.md's "PWM_MODE = 3x (Ctrl Reg2 bits[2:1] = 10b)" note. */
#define DRV8316_CTRL2_PWM_MODE_POS 1U
#define DRV8316_CTRL2_PWM_MODE_3X  (0x2U << DRV8316_CTRL2_PWM_MODE_POS)
/* Control_2 bit0: write 1 to clear latched faults. Many gate drivers refuse
 * new config writes while a fault is latched -- try this before config if
 * nFAULT is asserted and writes aren't sticking. */
#define DRV8316_CTRL2_CLR_FLT_BIT  0U

/* Control_6 (0x08): BUCK_DIS, bit0. Writing 1 disables the internal buck --
 * the mitigation for the known-bad 47uH-instead-of-22ohm FB_BK network. */
#define DRV8316_CTRL6_BUCK_DIS_BIT 0U

typedef struct
{
  GPIO_TypeDef *cs_port;
  uint16_t      cs_pin;
} DRV8316_Handle;

uint8_t DRV8316_ReadReg(const DRV8316_Handle *h, uint8_t addr);
void    DRV8316_WriteReg(const DRV8316_Handle *h, uint8_t addr, uint8_t data);

/* Pulses CLR_FLT (Control_2 bit0) to attempt clearing a latched fault.
 * Harmless to call even if there's no fault. Does not touch DRVOFF/PWM. */
void DRV8316_ClearFaults(const DRV8316_Handle *h);

/* Writes PWM_MODE=3x (Control_2) and BUCK_DIS=1 (Control_6) -- two separate
 * registers, not one combined write -- and reads both back. Returns true
 * only if both readbacks match what was written. */
bool DRV8316_ConfigureBuckDisablePwmMode(const DRV8316_Handle *h);

#endif /* DRV8316_H */
