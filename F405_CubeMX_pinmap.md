# STM32F405RGT6 — CubeMX Pin Configuration (Micromouse)

Reference for setting up the F405 in STM32CubeIDE + classic CubeMX.
No external crystal (HSI → PLL → 168 MHz). Debug = Serial Wire.

---

## Pin configuration

| Pin | Port | Signal (net) | CubeMX peripheral / mode | User Label | Config notes |
|---|---|---|---|---|---|
| 8 | PC0 | M1_ISENSE_A | ADC1_IN10 (injected) | M1_ISA | Injected rank, TIM1 TRGO trigger |
| 9 | PC1 | M1_ISENSE_B | ADC1_IN11 (injected) | M1_ISB | Injected rank |
| 10 | PC2 | M1_ISENSE_C | ADC1_IN12 (injected) | M1_ISC | Injected rank |
| 11 | PC3 | IR_RX1 | ADC1_IN13 | IR_RX1 | Regular conversion, long sample |
| 14 | PA0 | M2_ISENSE_A | ADC2_IN0 (injected) | M2_ISA | Injected rank, TIM8 TRGO |
| 15 | PA1 | M2_ISENSE_B | ADC2_IN1 (injected) | M2_ISB | Injected rank |
| 16 | PA2 | M2_ISENSE_C | ADC2_IN2 (injected) | M2_ISC | Injected rank |
| 17 | PA3 | IR_RX6 | ADC1_IN3 | IR_RX6 | Regular conversion |
| 20 | PA4 | VBAT_SENSE | ADC1_IN4 | VBAT_SENSE | Regular, slow; divider 10k/3.3k |
| 21 | PA5 | ENC_SCK | SPI1_SCK | ENC_SCK | SPI1 master, encoders |
| 22 | PA6 | ENC_MISO | SPI1_MISO | ENC_MISO | SPI1 |
| 23 | PA7 | ENC_MOSI | SPI1_MOSI | ENC_MOSI | SPI1 |
| 24 | PC4 | IR_RX2 | ADC2_IN14 | IR_RX2 | Regular conversion |
| 25 | PC5 | IR_RX3 | ADC2_IN15 | IR_RX3 | Regular conversion |
| 26 | PB0 | IR_RX4 | ADC1_IN8 | IR_RX4 | Regular conversion |
| 27 | PB1 | IR_RX5 | ADC1_IN9 | IR_RX5 | Regular conversion |
| 28 | PB2 | (BOOT1/spare) | GPIO / unused | BOOT1_SPARE | No-connect or spare GPIO |
| 29 | PB10 | IMU_CS | GPIO_Output | IMU_CS | Idle HIGH, SPI2 chip select |
| 30 | PB11 | DRV2_CS | GPIO_Output | DRV2_CS | Idle HIGH, driver 2 CS |
| 33 | PB12 | DRV1_CS | GPIO_Output | DRV1_CS | Idle HIGH, driver 1 CS |
| 34 | PB13 | SPI2_SCK | SPI2_SCK | SPI2_SCK | Shared: 2 drivers + IMU |
| 35 | PB14 | SPI2_MISO | SPI2_MISO | SPI2_MISO | Shared bus; 10k pull-up (bodge) |
| 36 | PB15 | SPI2_MOSI | SPI2_MOSI | SPI2_MOSI | Shared bus |
| 37 | PC6 | M2_INHA | TIM8_CH1 | M2_INHA | Center-aligned PWM |
| 38 | PC7 | M2_INHB | TIM8_CH2 | M2_INHB | PWM |
| 39 | PC8 | M2_INHC | TIM8_CH3 | M2_INHC | PWM |
| 40 | PC9 | NFAULT | GPIO_Input / EXTI | NFAULT | 10k pull-up to 3V3 (bodge); open-drain in |
| 41 | PA8 | M1_INHA | TIM1_CH1 | M1_INHA | Center-aligned PWM, TIM1 master |
| 42 | PA9 | M1_INHB | TIM1_CH2 | M1_INHB | PWM |
| 43 | PA10 | M1_INHC | TIM1_CH3 | M1_INHC | PWM |
| 44 | PA11 | IMU_INT | GPIO_EXTI | IMU_INT | Data-ready interrupt input |
| 45 | PA12 | ENC2_CS | GPIO_Output | ENC2_CS | Idle HIGH, encoder 2 CS |
| 46 | PA13 | SWDIO | SYS_JTMS-SWDIO | SWDIO | Debug = Serial Wire |
| 49 | PA14 | SWCLK | SYS_JTCK-SWCLK | SWCLK | Debug = Serial Wire |
| 50 | PA15 | ENC1_CS | GPIO_Output | ENC1_CS | Idle HIGH, encoder 1 CS |
| 51 | PC10 | LED_RED | GPIO_Output | LED_RED | Push-pull; verify actual color |
| 52 | PC11 | LED_GRN | GPIO_Output | LED_GRN | Push-pull |
| 53 | PC12 | LED_BLUE | GPIO_Output | LED_BLU | Push-pull |
| 2 | PC13 | NSLEEP | GPIO_Output | NSLEEP | Drive HIGH early (wake); low-drive pin |
| 3 | PC14 | DRVOFF | GPIO_Output | DRVOFF | Drive safe-state early; low-drive pin |
| 4 | PC15 | (spare) | GPIO / unused | SPARE_PC15 | No-connect / spare |
| 5 | PH0 | BTN_START | GPIO_Input | BTN_START | Internal pull-up; HSE off |
| 6 | PH1 | BTN_MODE | GPIO_Input | BTN_MODE | Internal pull-up; HSE off |
| 55 | PB3 | IR_EM1 | GPIO_Output | IR_EM1 | JTAG pin — set SWD-only; gate drive |
| 56 | PB4 | IR_EM2 | GPIO_Output | IR_EM2 | JTAG pin — SWD-only; gate drive |
| 57 | PB5 | IR_EM3 | GPIO_Output | IR_EM3 | Gate drive pulse |
| 58 | PB6 | IR_EM4 | GPIO_Output | IR_EM4 | Gate drive pulse |
| 59 | PB7 | IR_EM5 | GPIO_Output | IR_EM5 | Gate drive pulse |
| 60 | BOOT0 | BOOT0 | (boot pin) | BOOT0 | 10k pulldown to GND; boot from flash |
| 61 | PB8 | TAIL_PWM | TIMx_CHx or GPIO | TAIL_PWM | PWM to tail-light MOSFET gate |
| 62 | PB9 | BUZZ_PWM | TIMx_CHx | BUZZ_PWM | PWM to buzzer MOSFET gate |
| 54 | PD2 | IR_EM6 | GPIO_Output | IR_EM6 | Gate drive pulse |
| 1 | VBAT | VBAT→3V3 | (supply) | VBAT_MCU | Tied to 3V3 + 100nF (no coin cell) |
| 7 | NRST | NRST | (reset) | NRST | 100nF to GND; to SWD header |
| 13 | VDDA | VDDA | (supply) | VDDA | 3V3 via ferrite bead + 1uF/100nF |
| 12 | VSSA | VSSA | (ground) | VSSA | To GND plane |
| 31 | VCAP_1 | VCAP_1 | (internal reg) | VCAP1 | 2.2uF at pin; no trace to VCAP_2 |
| 47 | VCAP_2 | VCAP_2 | (internal reg) | VCAP2 | 2.2uF at pin |
| 19/32/48/64 | VDD | VDD | (supply) | VDD | 100nF each pin + 4.7uF bulk |
| 18/63 | VSS | VSS | (ground) | VSS | To GND plane |

---

## System clock

| Setting | Value / Note |
|---|---|
| Source | HSI (16 MHz internal) — no external crystal |
| PLL | HSI → PLL → 168 MHz SYSCLK |
| PLL config | M=8, N=168, P=2 (=168 MHz), Q=7 (48 MHz for USB if needed) |
| AHB | 168 MHz |
| APB1 | 42 MHz (timers ×2 = 84 MHz) |
| APB2 | 84 MHz (timers ×2 = 168 MHz) |
| Debug | Serial Wire (SYS → Debug = Serial Wire) |

---

## Timers — motor PWM

- **TIM1** — Motor 1, center-aligned PWM, CH1/2/3 = PA8/9/10. **MASTER** (TRGO on update).
- **TIM8** — Motor 2, center-aligned PWM, CH1/2/3 = PC6/7/8. **SLAVE** (sync to TIM1 via ITR0).
- **PWM freq** — ~20–30 kHz typical for FOC (set ARR accordingly).
- **Trigger** — TIM1 TRGO triggers ADC injected conversion at counter bottom (quiet point).

---

## ADC — current sense + sensors

- **ADC1** — Injected: M1_ISENSE A/B/C (PC0/1/2). Regular: IR_RX + VBAT_SENSE.
- **ADC2** — Injected: M2_ISENSE A/B/C (PA0/1/2).
- **Injected trigger** — TIM1 TRGO (M1), TIM8 TRGO (M2) — sample at PWM bottom.
- **DMA** — Regular conversions via DMA (circular) for IR / battery.

> CubeMX shows the valid ADC channel per pin when selected. If it flags a conflict, trust CubeMX — some IR_RX pins can land on ADC1 or ADC2 depending on how you balance injected/regular groups.

---

## SPI

- **SPI1** — Encoders (AS5047P ×2). Master, Mode 1 (CPOL 0 / CPHA 1), up to ~10 MHz.
- **SPI2** — Drivers (DRV8316 ×2) + IMU (ICM-42670). Master. CS: PB12 / PB11 / PB10.

---

## Driver init order (firmware — first actions)

1. Drive **NSLEEP (PC13) HIGH** — wake driver.
2. Drive **DRVOFF (PC14)** to safe state (HIGH = motors off until ready).
3. SPI2: DRV8316 config — **BUCK_DIS = 1** (disable internal buck).
4. SPI2: DRV8316 **PWM_MODE = 3×** (Ctrl Reg2 bits[2:1] = 10b).
5. Confirm **INLA/B/C = AVDD** (hardware) for 3× mode.
6. Read **nFAULT / fault registers** — confirm no fault, not in test mode.
7. Then enable PWM, run encoder alignment, start FOC.

---

## GPIO notes

- **PC13 / PC14** — plain GPIO, NOT LSE/RTC. Low-drive pins, OK for static control.
- **PH0 / PH1** — GPIO buttons; requires HSE disabled (no crystal, so fine).
- **PB3 / PB4** — JTAG pins; set Debug = Serial Wire to free them as GPIO (IR_EM1/2).
- **PB2** — BOOT1; free GPIO or no-connect.
- **BOOT0** — 10k pulldown; boots from flash.
