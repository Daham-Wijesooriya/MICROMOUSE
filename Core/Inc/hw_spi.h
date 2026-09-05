/**
 ******************************************************************************
 * @file    hw_spi.h
 * @brief   Minimal polled, register-level SPI1/SPI2 driver for bring-up.
 *
 * The vendor stm32f4xx_hal_spi.{c,h} files are NOT present in Drivers/ (this
 * project only carries the HAL modules CubeMX had previously generated:
 * ADC/GPIO/RCC/CORTEX/DMA/EXTI/FLASH/PWR). Rather than vendoring in the full
 * HAL SPI driver, this talks to SPI1/SPI2 directly via CMSIS register
 * definitions. It is intentionally simple: polled, 8-bit hardware frames,
 * software NSS (CS is a plain GPIO toggled by the caller so multiple devices
 * can share SPI2 with a decoder-free chip-select scheme).
 *
 * SPI1 = encoder bus (AS5047P x2)         -> APB2 = 84 MHz
 * SPI2 = driver + IMU bus (DRV8316 x2, ICM-42670) -> APB1 = 42 MHz
 *
 * Both peripherals are configured for CPOL=0 / CPHA=1 (SPI "Mode 1"), per
 * the encoder note in F405_CubeMX_pinmap.md. SPI2's mode is confirmed
 * correct for the DRV8316 against a working reference driver -- see
 * drv8316.h. ICM-42670's mode is still unconfirmed -- see icm42670.h.
 ******************************************************************************
 */
#ifndef HW_SPI_H
#define HW_SPI_H

#include "main.h"

/* Conservative first-bring-up clock: APB2/32 = 2.625 MHz (AS5047P max 10 MHz). */
void SPI1_HW_Init(void);

/* APB1/32 = 1.3125 MHz -- close to the 1 MHz a known-working DRV8316
 * reference driver uses, well under its ~5 MHz max, shared by 3 devices. */
void SPI2_HW_Init(void);

uint8_t  SPI1_TxRx8(uint8_t tx);
uint16_t SPI1_TxRx16(uint16_t tx);   /* MSB-first, two 8-bit hw frames */

uint8_t  SPI2_TxRx8(uint8_t tx);
uint16_t SPI2_TxRx16(uint16_t tx);   /* MSB-first, two 8-bit hw frames */

/* Small helpers so CS handling reads the same everywhere it's used. */
static inline void SPI_CS_Low(GPIO_TypeDef *port, uint16_t pin)  { HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); }
static inline void SPI_CS_High(GPIO_TypeDef *port, uint16_t pin) { HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); }

#endif /* HW_SPI_H */
