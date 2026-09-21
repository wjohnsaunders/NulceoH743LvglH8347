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
#include "stm32h7xx_hal.h"

#include "stm32h7xx_nucleo.h"
#include <stdio.h>

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
#define LCD_CS_Pin GPIO_PIN_3
#define LCD_CS_GPIO_Port GPIOF
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOF
#define LCD_WR_Pin GPIO_PIN_0
#define LCD_WR_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_3
#define LCD_RS_GPIO_Port GPIOC
#define LCD_RD_Pin GPIO_PIN_3
#define LCD_RD_GPIO_Port GPIOA
#define SD_SCK_Pin GPIO_PIN_5
#define SD_SCK_GPIO_Port GPIOA
#define SD_DO_Pin GPIO_PIN_6
#define SD_DO_GPIO_Port GPIOA
#define SD_DI_Pin GPIO_PIN_7
#define SD_DI_GPIO_Port GPIOA
#define LCD_D0_Pin GPIO_PIN_12
#define LCD_D0_GPIO_Port GPIOF
#define LCD_D7_Pin GPIO_PIN_13
#define LCD_D7_GPIO_Port GPIOF
#define LCD_D4_Pin GPIO_PIN_14
#define LCD_D4_GPIO_Port GPIOF
#define LCD_D2_Pin GPIO_PIN_15
#define LCD_D2_GPIO_Port GPIOF
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_11
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D3_Pin GPIO_PIN_13
#define LCD_D3_GPIO_Port GPIOE
#define SD_SS_Pin GPIO_PIN_14
#define SD_SS_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_15
#define LCD_D1_GPIO_Port GPIOD
#define JTMS_Pin GPIO_PIN_13
#define JTMS_GPIO_Port GPIOA
#define JTCK_Pin GPIO_PIN_14
#define JTCK_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
