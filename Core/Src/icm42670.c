/**
 ******************************************************************************
 * @file    icm42670.c
 * @brief   ICM-42670-P WHO_AM_I read over SPI2. See icm42670.h.
 ******************************************************************************
 */
#include "icm42670.h"
#include "hw_spi.h"

uint8_t ICM42670_ReadReg(uint8_t addr)
{
  SPI_CS_Low(IMU_CS_GPIO_Port, IMU_CS_Pin);
  (void)SPI2_TxRx8((uint8_t)(addr | 0x80U)); /* MSB set = read */
  uint8_t data = SPI2_TxRx8(0x00U);          /* dummy clock to shift data out */
  SPI_CS_High(IMU_CS_GPIO_Port, IMU_CS_Pin);
  return data;
}

uint8_t ICM42670_ReadWhoAmI(void)
{
  return ICM42670_ReadReg(ICM42670_REG_WHO_AM_I);
}
