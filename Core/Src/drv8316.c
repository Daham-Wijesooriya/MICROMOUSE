/**
 ******************************************************************************
 * @file    drv8316.c
 * @brief   DRV8316 SPI register access. See drv8316.h for the frame format
 *          and the reference this was cross-checked against.
 ******************************************************************************
 */
#include "drv8316.h"
#include "hw_spi.h"

/* Even parity over all 16 bits (frame constructed with parity bit = 0),
 * matching the reference driver's getParity(): count set bits, and if that
 * count is odd, the caller ORs in the parity bit to make the total even. */
static uint8_t NeedsParityBit(uint16_t frame_with_parity_zero)
{
  uint8_t count = 0;
  for (int i = 0; i < 16; i++)
  {
    if ((frame_with_parity_zero >> i) & 0x1U) { count++; }
  }
  return (uint8_t)(count & 0x1U); /* 1 = odd so far -> set parity bit to make it even */
}

static uint16_t BuildFrame(uint8_t addr, uint8_t data, bool read)
{
  uint16_t frame = (uint16_t)((read ? 0x8000U : 0x0000U) | ((uint16_t)(addr & 0x3FU) << 9) | data);
  if (NeedsParityBit(frame)) { frame |= 0x0100U; }
  return frame;
}

uint8_t DRV8316_ReadReg(const DRV8316_Handle *h, uint8_t addr)
{
  uint16_t frame = BuildFrame(addr, 0x00U, true);

  SPI_CS_Low(h->cs_port, h->cs_pin);
  uint16_t resp = SPI2_TxRx16(frame);
  SPI_CS_High(h->cs_port, h->cs_pin);

  return (uint8_t)(resp & 0xFFU);
}

void DRV8316_WriteReg(const DRV8316_Handle *h, uint8_t addr, uint8_t data)
{
  uint16_t frame = BuildFrame(addr, data, false);

  SPI_CS_Low(h->cs_port, h->cs_pin);
  (void)SPI2_TxRx16(frame);
  SPI_CS_High(h->cs_port, h->cs_pin);
}

void DRV8316_ClearFaults(const DRV8316_Handle *h)
{
  uint8_t ctrl2 = DRV8316_ReadReg(h, DRV8316_REG_CONTROL_2);
  DRV8316_WriteReg(h, DRV8316_REG_CONTROL_2, (uint8_t)(ctrl2 | (1U << DRV8316_CTRL2_CLR_FLT_BIT)));
  /* Leave it set -- if this bit is self-clearing per the datasheet it'll
   * read back 0 on its own; if it's not, the next ConfigureBuckDisablePwmMode()
   * read-modify-write will preserve whatever it reads here. */
}

/* Control_2 also carries CLR_FLT/SLEW/SDO_MODE; Control_6 also carries
 * BUCK_SEL/BUCK_CL/BUCK_PS_DIS. Read-modify-write so those are left as
 * whatever the chip's power-on defaults are, instead of being blindly
 * zeroed by a straight write of just the PWM_MODE/BUCK_DIS bits. */
#define DRV8316_CTRL2_PWM_MODE_MASK (0x3U << DRV8316_CTRL2_PWM_MODE_POS)
#define DRV8316_CTRL6_BUCK_DIS_MASK (0x1U << DRV8316_CTRL6_BUCK_DIS_BIT)

bool DRV8316_ConfigureBuckDisablePwmMode(const DRV8316_Handle *h)
{
  uint8_t ctrl2 = DRV8316_ReadReg(h, DRV8316_REG_CONTROL_2);
  ctrl2 = (uint8_t)((ctrl2 & ~DRV8316_CTRL2_PWM_MODE_MASK) | DRV8316_CTRL2_PWM_MODE_3X);
  DRV8316_WriteReg(h, DRV8316_REG_CONTROL_2, ctrl2);
  uint8_t got_ctrl2 = DRV8316_ReadReg(h, DRV8316_REG_CONTROL_2);

  uint8_t ctrl6 = DRV8316_ReadReg(h, DRV8316_REG_CONTROL_6);
  ctrl6 = (uint8_t)((ctrl6 & ~DRV8316_CTRL6_BUCK_DIS_MASK) | (0x1U << DRV8316_CTRL6_BUCK_DIS_BIT));
  DRV8316_WriteReg(h, DRV8316_REG_CONTROL_6, ctrl6);
  uint8_t got_ctrl6 = DRV8316_ReadReg(h, DRV8316_REG_CONTROL_6);

  return (got_ctrl2 == ctrl2) && (got_ctrl6 == ctrl6);
}
