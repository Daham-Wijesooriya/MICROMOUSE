/**
 ******************************************************************************
 * @file    ir_sense.h
 * @brief   Raw single-conversion ADC reads for the 6 IR_RX channels, and
 *          gate-drive pulses for the 6 IR_EM emitters. Step 6f of the
 *          bring-up plan: read baseline with emitters off, then pulse each
 *          emitter and confirm its matched receiver responds.
 ******************************************************************************
 */
#ifndef IR_SENSE_H
#define IR_SENSE_H

#include "main.h"
#include <stdbool.h>

#define IR_CHANNEL_COUNT 6

/* Enables GPIOD/IR_EM6 (not covered by MX_GPIO_Init) and starts the DWT
 * cycle counter used for microsecond emitter pulse timing. Call once. */
void IR_Init(void);

/* Index order: RX1..RX6 / EM1..EM6, matching F405_CubeMX_pinmap.md. */
void IR_ReadAllRaw(uint16_t out[IR_CHANNEL_COUNT]);

/* Drives IR_EM[idx] high (on=true) or low (on=false). idx is 0-based (0 = EM1). */
void IR_SetEmitter(uint8_t idx, bool on);

/* Drives IR_EM[idx] high for pulse_us then low again (scope-friendly pulse,
 * no ADC read in between -- see bringup.c for the "on, sample RX, off" path). */
void IR_PulseEmitter(uint8_t idx, uint32_t pulse_us);

#endif /* IR_SENSE_H */
