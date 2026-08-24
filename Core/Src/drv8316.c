/**
 ******************************************************************************
 * @file    drv8316.c
 * @brief   DRV8316 SPI register access. See drv8316.h for the register-map
 *          verification warning -- do not trust this beyond "exercises the
 *          SPI bus" until the addresses/bit positions are confirmed.
 ******************************************************************************
 */
#include "drv8316.h"
#include "hw_spi.h"

uint16_t DRV8316_ReadReg(const DRV8316_Handle *h, uint8_t addr)
{
  /* bit15=1 assumed to mean "read" (TI DRV83xx-family convention) -- also
   * unverified against the DRV8316 datasheet, same caveat as drv8316.h. */
  uint16_t frame = (uint16_t)(0x8000U | ((addr & 0x0FU) << 11));

  SPI_CS_Low(h->cs_port, h->cs_pin);
  uint16_t resp = SPI2_TxRx16(frame);
  SPI_CS_High(h->cs_port, h->cs_pin);

  return (uint16_t)(resp & 0x07FFU);
}

void DRV8316_WriteReg(const DRV8316_Handle *h, uint8_t addr, uint16_t data11)
{
  uint16_t frame = (uint16_t)(((addr & 0x0FU) << 11) | (data11 & 0x07FFU));

  SPI_CS_Low(h->cs_port, h->cs_pin);
  (void)SPI2_TxRx16(frame);
  SPI_CS_High(h->cs_port, h->cs_pin);
}

bool DRV8316_ConfigureBuckDisablePwmMode(const DRV8316_Handle *h)
{
  uint16_t want = (uint16_t)((1U << DRV8316_CTRL2_BUCK_DIS_BIT) | DRV8316_CTRL2_PWM_MODE_3X);

  DRV8316_WriteReg(h, DRV8316_REG_CTRL2, want);
  uint16_t got = DRV8316_ReadReg(h, DRV8316_REG_CTRL2);

  return (got == want);
}
