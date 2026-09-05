/**
 ******************************************************************************
 * @file    io_selftest.h
 * @brief   Standalone BTN_START/BTN_MODE -> RGB LED smoke test.
 *
 * Purpose: confirm a fresh flash actually took, and that button/LED wiring
 * plus basic GPIO input/output work, before the heavier bring-up sequencer
 * (which depends on SPI/ADC and external chips) runs. Touches nothing else.
 *
 *   idle (neither button held) -> LED_BLUE blinking (proves the firmware is
 *                                 alive and not stuck at reset/hard fault)
 *   BTN_START held              -> LED_RED solid
 *   BTN_MODE held               -> LED_GRN solid
 *   both held                   -> all three LEDs solid
 ******************************************************************************
 */
#ifndef IO_SELFTEST_H
#define IO_SELFTEST_H

#include "main.h"

/* Runs for duration_ms, polling buttons and driving LEDs as above, then
 * turns all LEDs off and returns. */
void IoSelfTest_Run(uint32_t duration_ms);

#endif /* IO_SELFTEST_H */
