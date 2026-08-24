/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NSLEEP_Pin GPIO_PIN_13
#define NSLEEP_GPIO_Port GPIOC
#define DRVOFF_Pin GPIO_PIN_14
#define DRVOFF_GPIO_Port GPIOC
#define BTN_START_Pin GPIO_PIN_0
#define BTN_START_GPIO_Port GPIOH
#define BTN_MODE_Pin GPIO_PIN_1
#define BTN_MODE_GPIO_Port GPIOH
#define M1_ISENSE_A_Pin GPIO_PIN_0
#define M1_ISENSE_A_GPIO_Port GPIOC
#define M1_ISENSE_B_Pin GPIO_PIN_1
#define M1_ISENSE_B_GPIO_Port GPIOC
#define M1_ISENSE_C_Pin GPIO_PIN_2
#define M1_ISENSE_C_GPIO_Port GPIOC
#define IR_RX1_Pin GPIO_PIN_3
#define IR_RX1_GPIO_Port GPIOC
#define M2_ISENSE_A_Pin GPIO_PIN_0
#define M2_ISENSE_A_GPIO_Port GPIOA
#define M2_ISENSE_B_Pin GPIO_PIN_1
#define M2_ISENSE_B_GPIO_Port GPIOA
#define M2_ISENSE_C_Pin GPIO_PIN_2
#define M2_ISENSE_C_GPIO_Port GPIOA
#define IR_RX6_Pin GPIO_PIN_3
#define IR_RX6_GPIO_Port GPIOA
#define VBAT_SENSE_Pin GPIO_PIN_4
#define VBAT_SENSE_GPIO_Port GPIOA
#define ENC_SCK_Pin GPIO_PIN_5
#define ENC_SCK_GPIO_Port GPIOA
#define ENC_MISO_Pin GPIO_PIN_6
#define ENC_MISO_GPIO_Port GPIOA
#define ENC_MOSI_Pin GPIO_PIN_7
#define ENC_MOSI_GPIO_Port GPIOA
#define IR_RX2_Pin GPIO_PIN_4
#define IR_RX2_GPIO_Port GPIOC
#define IR_RX3_Pin GPIO_PIN_5
#define IR_RX3_GPIO_Port GPIOC
#define IR_RX4_Pin GPIO_PIN_0
#define IR_RX4_GPIO_Port GPIOB
#define IR_RX5_Pin GPIO_PIN_1
#define IR_RX5_GPIO_Port GPIOB
#define IMU_CS_Pin GPIO_PIN_10
#define IMU_CS_GPIO_Port GPIOB
#define DRV2_CS_Pin GPIO_PIN_11
#define DRV2_CS_GPIO_Port GPIOB
#define DRV1_CS_Pin GPIO_PIN_12
#define DRV1_CS_GPIO_Port GPIOB
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define M2_INHA_Pin GPIO_PIN_6
#define M2_INHA_GPIO_Port GPIOC
#define M2_INHB_Pin GPIO_PIN_7
#define M2_INHB_GPIO_Port GPIOC
#define M2_INHC_Pin GPIO_PIN_8
#define M2_INHC_GPIO_Port GPIOC
#define NFAULT_Pin GPIO_PIN_9
#define NFAULT_GPIO_Port GPIOC
#define M1_INHA_Pin GPIO_PIN_8
#define M1_INHA_GPIO_Port GPIOA
#define M1_INHB_Pin GPIO_PIN_9
#define M1_INHB_GPIO_Port GPIOA
#define M1_INHC_Pin GPIO_PIN_10
#define M1_INHC_GPIO_Port GPIOA
#define IMU_INT_Pin GPIO_PIN_11
#define IMU_INT_GPIO_Port GPIOA
#define ENC2_CS_Pin GPIO_PIN_12
#define ENC2_CS_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define ENC1_CS_Pin GPIO_PIN_15
#define ENC1_CS_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_10
#define LED_RED_GPIO_Port GPIOC
#define LED_GRN_Pin GPIO_PIN_11
#define LED_GRN_GPIO_Port GPIOC
#define LED_BLUE_Pin GPIO_PIN_12
#define LED_BLUE_GPIO_Port GPIOC
#define IR_EM1_Pin GPIO_PIN_3
#define IR_EM1_GPIO_Port GPIOB
#define IR_EM2_Pin GPIO_PIN_4
#define IR_EM2_GPIO_Port GPIOB
#define IR_EM3_Pin GPIO_PIN_5
#define IR_EM3_GPIO_Port GPIOB
#define IR_EM4_Pin GPIO_PIN_6
#define IR_EM4_GPIO_Port GPIOB
#define IR_EM5_Pin GPIO_PIN_7
#define IR_EM5_GPIO_Port GPIOB
#define TAIL_PWM_Pin GPIO_PIN_8
#define TAIL_PWM_GPIO_Port GPIOB
#define IR_EM6_Pin GPIO_PIN_2
#define IR_EM6_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
