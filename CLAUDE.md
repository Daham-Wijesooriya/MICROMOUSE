# CLAUDE.md

Guidance for Claude Code when working in this repository.

## What this is

Firmware for a custom STM32F405RGT6-based micromouse robot (PCB-frame chassis, dual
brushless/FOC drivetrain). The board has been assembled but not yet fully validated on the
bench — see "Hardware status" below before assuming any peripheral is known-good.

Toolchain: STM32CubeIDE + classic CubeMX (F405, F4-series). Build system is CMake + Ninja +
`arm-none-eabi-gcc`, generated to mirror what CubeIDE produces (see [README.md](README.md)
for build/flash commands). There is **no `.ioc` file checked into this repo** — pin/clock
config lives in [F405_CubeMX_pinmap.md](F405_CubeMX_pinmap.md), which is the source of truth
for pin assignments, clock tree, and peripheral init order. Read it before touching any
peripheral init code.

## Hardware status (as of 2026-08-24)

Known open issues on the assembled board — do not assume these are resolved unless told
otherwise:
- Previously measured ~11 Ω between 3V3 and GND (near-short), suspected QFN solder bridge on
  one of the two DRV8316 drivers. Root cause status unknown at any given session — ask before
  assuming it's fixed.
- Missing pull resistors from schematic, bodged in: `nFAULT` (PC9) needs 10k pull-up,
  `SPI2_MISO` (PB14) needs 10k pull-up.
- Buck network issue on the DRV8316 `FB_BK` net: a 47 µH inductor is populated where a 22 Ω
  resistor belongs. Mitigation is `BUCK_DIS=1` written over SPI before the driver is ever
  enabled — this is enforced in firmware (see below) but the DRV8316 register map used to do
  it is **unverified** (see `Core/Inc/drv8316.h`).
- No external crystal — clock is HSI → PLL → 168 MHz only, no HSE/LSE.

There is no UART on this board. Bring-up test results are surfaced two ways: an RGB LED
status code, and a global `g_bringup` struct (in `Core/Src/bringup.c`) meant to be inspected
live with a debugger — there is no other logging path.

## Firmware architecture

`Core/Src/main.c` runs, in order: a button/LED GPIO smoke test, a peripheral bring-up test
sequencer, then a gated open-loop motor spin test. None of this is application logic yet.

0. `IoSelfTest_Run()` — 10s of mirroring BTN_START/BTN_MODE onto the RGB LED, touching nothing
   else (no SPI/ADC/PWM). Exists purely to give immediate visual confirmation that a fresh
   flash took and basic GPIO works, before anything depending on external chips runs.
1. `Bringup_RunAll()` — NSLEEP/DRVOFF safe-state check → SPI2 + DRV8316 register access/
   BUCK_DIS config → SPI1 + AS5047P encoder read → ICM-42670 WHO_AM_I → IR sensor raw ADC +
   emitter pulse test. Halts internally (never returns) on a hard fail; otherwise shows a
   brief LED summary and returns.
2. `MotorTest_RunGated()` — the step-6g open-loop spin test, added on explicit request. It is
   NOT closed-loop FOC (no Clarke/Park, no current loop, no encoder feedback) — it forces a
   slow, low-amplitude rotating voltage vector on one motor at a time. It blocks forever
   handling arm gestures; see its file header for the full safety mechanism (per-driver
   BUCK_DIS-confirmed gate, 2 s hold-to-arm, abortable countdown, live nFAULT + two-button
   abort chord, fixed short ramp/hold/ramp profile, single motor only). It cannot see smoke or
   watch a bench PSU's current display — a human still has to do that, with wheels off the
   ground and the supply current-limited, per the bring-up plan.

Custom modules added on top of the CubeMX-generated `Core`/`Drivers` skeleton:
- `hw_spi.{h,c}` — polled, register-level SPI1/SPI2 driver. The vendor
  `stm32f4xx_hal_spi.{c,h}` files are **not present** in `Drivers/` (this project only carries
  the HAL modules CubeMX had generated before: ADC/GPIO/RCC/CORTEX/DMA/EXTI/FLASH/PWR).
  Rather than vendoring in the full HAL SPI driver, SPI1/SPI2 are driven directly via CMSIS
  registers. If you add HAL_SPI later, this file becomes redundant — remove it, don't run both.
- `motor_pwm.{h,c}` — same rationale, for TIM1 (M1)/TIM8 (M2): no vendor `hal_tim.{c,h}`
  present, so PWM is register-level. Only CH1/2/3 are driven (DRV8316s are wired for "3x PWM"
  mode, INLA/B/C tied to AVDD in hardware) with `BDTR.MOE` as the fast output-disable path.
- `drv8316.{h,c}`, `as5047p.{h,c}`, `icm42670.{h,c}`, `ir_sense.{h,c}` — one file pair per
  external chip/sensor group, register access only.
- `bringup.{h,c}` — stage 1 above.
- `motor_test.{h,c}` — stage 2 above.

**`drv8316.h`'s frame format and register map were originally uninformed placeholders and
were wrong** (no parity bit, wrong address width, `BUCK_DIS`/`PWM_MODE` assumed to share one
register when they don't). As of 2026-09-05 they're rewritten cross-checked against a working
open-source DRV8316 SPI driver (SimpleFOC's `Arduino-FOC-drivers`, `src/drivers/drv8316/`) —
better-grounded than a guess, but still not verified against the TI datasheet directly (that
PDF hasn't extracted cleanly). If register behavior still looks wrong on hardware, that's the
next place to check. Same unverified-lower-severity caveat still applies to `icm42670.h`'s
WHO_AM_I expected value and `motor_pwm.h`'s `PWM_NEUTRAL_PCT` (50% duty = ~0 V phase output)
assumption — neither has a reference implementation to check against yet.

## Firmware invariant — do not regress this

`DRVOFF` (PC14) must default HIGH (safe/Hi-Z) at boot and only ever go LOW immediately before
enabling PWM, after `BUCK_DIS` is confirmed written over SPI2 **and** `nFAULT` is confirmed
clear. `main.c` previously had a bug where `DRVOFF` was pulled low right after `NSLEEP`, before
any SPI configuration — that would enable driver outputs through the known-bad buck network
with zero mitigation applied. It's fixed now: `DRVOFF` stays high through the whole bring-up
sequencer, and `motor_test.c`'s `TryArmAndRun()`/`RunOpenLoopSpin()` are the only places that
ever pull it low, gated on the mechanism described above. If you touch this path, preserve
that gating — don't make it easier to reach DRVOFF=LOW than "hold a button for 2s after a
clean per-driver BUCK_DIS confirmation."

## Conventions

- Keep the CubeMX `/* USER CODE BEGIN ... END */` markers intact in `Core/Src/main.c`,
  `Core/Src/stm32f4xx_hal_msp.c`, and `Core/Inc/main.h` — if a `.ioc` is ever added and
  regenerated from, code outside those markers gets overwritten.
- New peripheral source files go in `Core/Src` / `Core/Inc` (flat, matching the existing
  layout) and must be added to `cmake/files.cmake`'s `target_sources()` list or they won't
  build.
- Pin/signal names in code should match `F405_CubeMX_pinmap.md` and the `*_Pin`/`*_GPIO_Port`
  macros already defined in `main.h` — don't invent new names for the same net.
- Do not weaken, bypass, or remove the arm/countdown/abort gating in `motor_test.c`, and don't
  add new code paths that pull `DRVOFF` low or start motor PWM outside of it, without the user
  explicitly asking — it's the highest-risk action on this board given the open hardware
  issues above. Closed-loop FOC (current control, Clarke/Park) has not been requested or
  written yet — `motor_test.c` is intentionally open-loop only.
