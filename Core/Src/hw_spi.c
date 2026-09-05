/**
 ******************************************************************************
 * @file    hw_spi.c
 * @brief   Minimal polled, register-level SPI1/SPI2 driver. See hw_spi.h.
 ******************************************************************************
 */
#include "hw_spi.h"

/* BR[2:0]: prescaler = 2^(BR+1). BR=3 -> /16, BR=4 -> /32. */
#define SPI_BR_DIV16   (3U << SPI_CR1_BR_Pos)
#define SPI_BR_DIV32   (4U << SPI_CR1_BR_Pos)

static uint8_t SPI_TxRx8_Raw(SPI_TypeDef *spi, uint8_t tx)
{
  /* Wait for TX buffer empty, then send. */
  while (!(spi->SR & SPI_SR_TXE)) { }
  spi->DR = tx;

  /* Wait for RX buffer full, then read (also clears RXNE). */
  while (!(spi->SR & SPI_SR_RXNE)) { }
  uint8_t rx = (uint8_t)spi->DR;

  /* Wait for BSY to clear before the caller can safely toggle CS. */
  while (spi->SR & SPI_SR_BSY) { }

  return rx;
}

void SPI1_HW_Init(void)
{
  __HAL_RCC_SPI1_CLK_ENABLE();

  SPI1->CR1 = 0;
  SPI1->CR1 = SPI_CR1_MSTR       /* master              */
            | SPI_CR1_SSM | SPI_CR1_SSI  /* software NSS, forced high */
            | SPI_CR1_CPHA        /* CPOL=0, CPHA=1 -> Mode 1 (AS5047P) */
            | SPI_BR_DIV32;       /* 84 MHz / 32 = 2.625 MHz */
  SPI1->CR2 = 0;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI2_HW_Init(void)
{
  __HAL_RCC_SPI2_CLK_ENABLE();

  SPI2->CR1 = 0;
  /* Mode 1 (CPOL=0, CPHA=1) confirmed against SimpleFOC's working DRV8316
   * driver (SPI_MODE1) -- see drv8316.h. ICM-42670's mode is still
   * unconfirmed; if its reads look wrong, that's the first thing to try
   * flipping for that device specifically. */
  SPI2->CR1 = SPI_CR1_MSTR
            | SPI_CR1_SSM | SPI_CR1_SSI
            | SPI_CR1_CPHA        /* Mode 1: CPOL=0, CPHA=1 */
            | SPI_BR_DIV32;       /* 42 MHz / 32 = 1.3125 MHz, close to the
                                    * 1 MHz the reference DRV8316 driver uses
                                    * and well under its ~5 MHz max. */
  SPI2->CR2 = 0;
  SPI2->CR1 |= SPI_CR1_SPE;
}

uint8_t SPI1_TxRx8(uint8_t tx)  { return SPI_TxRx8_Raw(SPI1, tx); }
uint8_t SPI2_TxRx8(uint8_t tx)  { return SPI_TxRx8_Raw(SPI2, tx); }

uint16_t SPI1_TxRx16(uint16_t tx)
{
  uint16_t rx_hi = SPI_TxRx8_Raw(SPI1, (uint8_t)(tx >> 8));
  uint16_t rx_lo = SPI_TxRx8_Raw(SPI1, (uint8_t)(tx & 0xFF));
  return (uint16_t)((rx_hi << 8) | rx_lo);
}

uint16_t SPI2_TxRx16(uint16_t tx)
{
  uint16_t rx_hi = SPI_TxRx8_Raw(SPI2, (uint8_t)(tx >> 8));
  uint16_t rx_lo = SPI_TxRx8_Raw(SPI2, (uint8_t)(tx & 0xFF));
  return (uint16_t)((rx_hi << 8) | rx_lo);
}
