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

`Core/Src/main.c` currently runs a peripheral bring-up test sequencer, not application logic:
NSLEEP/DRVOFF safe-state check → SPI2 + DRV8316 register access/BUCK_DIS config → SPI1 +
AS5047P encoder read → ICM-42670 WHO_AM_I → IR sensor raw ADC + emitter pulse test. Motor PWM
(open-loop spin test) is **deliberately not automated** — that step needs a human watching
for smoke/current spikes in real time, per the bring-up plan, and must be done interactively.

Custom modules added on top of the CubeMX-generated `Core`/`Drivers` skeleton:
- `hw_spi.{h,c}` — polled, register-level SPI1/SPI2 driver. The vendor
  `stm32f4xx_hal_spi.{c,h}` files are **not present** in `Drivers/` (this project only carries
  the HAL modules CubeMX had generated before: ADC/GPIO/RCC/CORTEX/DMA/EXTI/FLASH/PWR).
  Rather than vendoring in the full HAL SPI driver, SPI1/SPI2 are driven directly via CMSIS
  registers. If you add HAL_SPI later, this file becomes redundant — remove it, don't run both.
- `drv8316.{h,c}`, `as5047p.{h,c}`, `icm42670.{h,c}`, `ir_sense.{h,c}` — one file pair per
  external chip/sensor group, register access only, no motor control logic yet.
- `bringup.{h,c}` — the test sequencer described above.

**`drv8316.h` register addresses/bit positions are explicitly flagged placeholders**, not
verified datasheet transcriptions — cross-check them against the actual DRV8316 datasheet
register table before trusting `DRV8316_ConfigureBuckDisablePwmMode()` to do what its name
says. Same caveat, lower severity, on `icm42670.h`'s WHO_AM_I expected value.

## Firmware invariant — do not regress this

`DRVOFF` (PC14) must default HIGH (safe/Hi-Z) at boot and only ever go LOW immediately before
enabling PWM, after `BUCK_DIS` is confirmed written over SPI2 **and** `nFAULT` is confirmed
clear. This project's `main.c` previously had a bug where `DRVOFF` was pulled low right after
`NSLEEP`, before any SPI configuration — that would enable driver outputs through the known-bad
buck network with zero mitigation applied. It's fixed now (`DRVOFF` stays high for the whole
bring-up sequencer); if you're adding motor-enable code, preserve the ordering.

## Conventions

- Keep the CubeMX `/* USER CODE BEGIN ... END */` markers intact in `Core/Src/main.c`,
  `Core/Src/stm32f4xx_hal_msp.c`, and `Core/Inc/main.h` — if a `.ioc` is ever added and
  regenerated from, code outside those markers gets overwritten.
- New peripheral source files go in `Core/Src` / `Core/Inc` (flat, matching the existing
  layout) and must be added to `cmake/files.cmake`'s `target_sources()` list or they won't
  build.
- Pin/signal names in code should match `F405_CubeMX_pinmap.md` and the `*_Pin`/`*_GPIO_Port`
  macros already defined in `main.h` — don't invent new names for the same net.
- Do not add code that pulls `DRVOFF` low or starts motor PWM without the user explicitly
  asking for that step — it's the highest-risk action on this board given the open hardware
  issues above.
