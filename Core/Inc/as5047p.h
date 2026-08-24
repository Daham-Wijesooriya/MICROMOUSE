/**
 ******************************************************************************
 * @file    as5047p.h
 * @brief   AS5047P magnetic rotary encoder, raw angle read over SPI1.
 *
 * Standard AMS AS5047/AS5048-family 16-bit frame:
 *   bit15 = even parity over bits[14:0]
 *   bit14 = R/W (1 = read, 0 = write)
 *   bits13:0 = 14-bit register address (command frame) or data (response frame)
 *
 * A read is two SPI transactions: send the command frame (address, RW=1),
 * then send a NOP and the PREVIOUS command's data comes back in that second
 * response frame. bit14 of the response is EF (error flag) -- if set, read
 * ERRFL (0x0001) to find out why.
 ******************************************************************************
 */
#ifndef AS5047P_H
#define AS5047P_H

#include "main.h"

#define AS5047P_REG_NOP      0x0000U
#define AS5047P_REG_ERRFL    0x0001U
#define AS5047P_REG_DIAAGC   0x3FFCU
#define AS5047P_REG_MAG      0x3FFDU
#define AS5047P_REG_ANGLEUNC 0x3FFEU
#define AS5047P_REG_ANGLECOM 0x3FFFU

typedef struct
{
  uint16_t angle_raw;   /* 14-bit, 0..16383 */
  uint8_t  error_flag;  /* response EF bit set -> check ERRFL          */
  uint8_t  parity_ok;   /* 1 if response frame parity was self-consistent */
} AS5047P_Reading;

/* One encoder's CS pin (ENC1_CS = PA15, ENC2_CS = PA12). */
AS5047P_Reading AS5047P_ReadAngle(GPIO_TypeDef *cs_port, uint16_t cs_pin);

/* Raw single-register read/write, exposed for reading ERRFL on fault. */
uint16_t AS5047P_ReadReg(GPIO_TypeDef *cs_port, uint16_t cs_pin, uint16_t addr);

#endif /* AS5047P_H */
