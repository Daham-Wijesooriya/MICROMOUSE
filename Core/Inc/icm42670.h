/**
 ******************************************************************************
 * @file    icm42670.h
 * @brief   ICM-42670-P IMU, WHO_AM_I check over SPI2.
 *
 * Standard TDK/InvenSense SPI framing: first byte = register address, MSB
 * set for a read (addr | 0x80); a write sends the data byte immediately
 * after the address byte with MSB clear.
 *
 * !!! VERIFY BEFORE TRUSTING !!!
 * WHO_AM_I register address (0x75) and expected ID (0x67) are this file's
 * best-effort recollection of the ICM-42670-P datasheet, not a verified
 * transcription. Cross-check both against the current datasheet's register
 * map before relying on a mismatch/match to mean anything.
 ******************************************************************************
 */
#ifndef ICM42670_H
#define ICM42670_H

#include "main.h"

#define ICM42670_REG_WHO_AM_I     0x75U
#define ICM42670_WHOAMI_EXPECTED  0x67U  /* TODO: confirm against datasheet */

uint8_t ICM42670_ReadReg(uint8_t addr);
uint8_t ICM42670_ReadWhoAmI(void);

#endif /* ICM42670_H */
