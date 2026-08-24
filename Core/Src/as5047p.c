/**
 ******************************************************************************
 * @file    as5047p.c
 * @brief   AS5047P raw angle read over SPI1. See as5047p.h.
 ******************************************************************************
 */
#include "as5047p.h"
#include "hw_spi.h"

static uint8_t EvenParityBit15(uint16_t frame_bits14_0)
{
  uint32_t v = frame_bits14_0;
  v ^= v >> 8;
  v ^= v >> 4;
  v ^= v >> 2;
  v ^= v >> 1;
  return (uint8_t)(v & 0x1U); /* 1 if odd number of set bits, i.e. the bit
                                 needed to make the total even */
}

static uint16_t BuildCommandFrame(uint16_t addr, uint8_t read)
{
  uint16_t body = (uint16_t)((read ? 0x4000U : 0x0000U) | (addr & 0x3FFFU));
  uint16_t par = EvenParityBit15(body);
  return (uint16_t)((par << 15) | body);
}

uint16_t AS5047P_ReadReg(GPIO_TypeDef *cs_port, uint16_t cs_pin, uint16_t addr)
{
  uint16_t cmd = BuildCommandFrame(addr, 1);

  SPI_CS_Low(cs_port, cs_pin);
  (void)SPI1_TxRx16(cmd);
  SPI_CS_High(cs_port, cs_pin);

  /* Second transaction (NOP) clocks out the response to `cmd`. */
  uint16_t nop = BuildCommandFrame(AS5047P_REG_NOP, 1);
  SPI_CS_Low(cs_port, cs_pin);
  uint16_t resp = SPI1_TxRx16(nop);
  SPI_CS_High(cs_port, cs_pin);

  return resp;
}

AS5047P_Reading AS5047P_ReadAngle(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
  AS5047P_Reading r;
  uint16_t resp = AS5047P_ReadReg(cs_port, cs_pin, AS5047P_REG_ANGLECOM);

  uint8_t rx_parity = (uint8_t)(resp >> 15);
  uint8_t expected_parity = EvenParityBit15(resp & 0x7FFFU);

  r.parity_ok  = (rx_parity == expected_parity) ? 1U : 0U;
  r.error_flag = (uint8_t)((resp >> 14) & 0x1U);
  r.angle_raw  = (uint16_t)(resp & 0x3FFFU);
  return r;
}
