/**
 ******************************************************************************
 * @file    io_selftest.c
 * @brief   BTN_START/BTN_MODE -> RGB LED smoke test. See io_selftest.h.
 ******************************************************************************
 */
#include "io_selftest.h"
#include <stdbool.h>

static void LedSet(bool r, bool g, bool b)
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void IoSelfTest_Run(uint32_t duration_ms)
{
  uint32_t start = HAL_GetTick();
  uint32_t last_blink = start;
  bool blink_on = false;

  while ((HAL_GetTick() - start) < duration_ms)
  {
    bool start_pressed = (HAL_GPIO_ReadPin(BTN_START_GPIO_Port, BTN_START_Pin) == GPIO_PIN_RESET);
    bool mode_pressed  = (HAL_GPIO_ReadPin(BTN_MODE_GPIO_Port, BTN_MODE_Pin) == GPIO_PIN_RESET);

    if (start_pressed && mode_pressed)
    {
      LedSet(true, true, true);
    }
    else if (start_pressed)
    {
      LedSet(true, false, false);
    }
    else if (mode_pressed)
    {
      LedSet(false, true, false);
    }
    else
    {
      if ((HAL_GetTick() - last_blink) > 200U)
      {
        blink_on = !blink_on;
        last_blink = HAL_GetTick();
      }
      LedSet(false, false, blink_on);
    }
  }

  LedSet(false, false, false);
}
