## F405 CubeMX Pin Config
| Pin | Port | Signal (net) | CubeMX peripheral / mode | User Label | Config notes | Group |
| --- | --- | --- | --- | --- | --- | --- |
| 8 | PC0 | M1\_ISENSE\_A | ADC1\_IN10 (injected) | M1\_ISA | Injected rank, TIM1 TRGO trigger | SENSE |
| 9 | PC1 | M1\_ISENSE\_B | ADC1\_IN11 (injected) | M1\_ISB | Injected rank | SENSE |
| 10 | PC2 | M1\_ISENSE\_C | ADC1\_IN12 (injected) | M1\_ISC | Injected rank | SENSE |
| 11 | PC3 | IR\_RX1 | ADC1\_IN13 | IR\_RX1 | Regular conversion, sample long | IR |
| 14 | PA0 | M2\_ISENSE\_A | ADC2\_IN0 (injected) | M2\_ISA | Injected rank, TIM8 TRGO | SENSE |
| 15 | PA1 | M2\_ISENSE\_B | ADC2\_IN1 (injected) | M2\_ISB | Injected rank | SENSE |
| 16 | PA2 | M2\_ISENSE\_C | ADC2\_IN2 (injected) | M2\_ISC | Injected rank | SENSE |
| 17 | PA3 | IR\_RX6 | ADC1\_IN3 | IR\_RX6 | Regular conversion | IR |
| 20 | PA4 | VBAT\_SENSE | ADC1\_IN4 | VBAT\_SENSE | Regular, slow, divider 10k/3.3k | SENSE |
| 21 | PA5 | ENC\_SCK | SPI1\_SCK | ENC\_SCK | SPI1 master, encoders | SPI |
| 22 | PA6 | ENC\_MISO | SPI1\_MISO | ENC\_MISO | SPI1 | SPI |
| 23 | PA7 | ENC\_MOSI | SPI1\_MOSI | ENC\_MOSI | SPI1 | SPI |
| 24 | PC4 | IR\_RX2 | ADC2\_IN14 | IR\_RX2 | Regular conversion | IR |
| 25 | PC5 | IR\_RX3 | ADC2\_IN15 | IR\_RX3 | Regular conversion | IR |
| 26 | PB0 | IR\_RX4 | ADC1\_IN8 | IR\_RX4 | Regular conversion | IR |
| 27 | PB1 | IR\_RX5 | ADC1\_IN9 | IR\_RX5 | Regular conversion | IR |
| 28 | PB2 | (BOOT1/spare) | GPIO / unused | BOOT1\_SPARE | No-connect or spare GPIO | IO |
| 29 | PB10 | IMU\_CS | GPIO\_Output | IMU\_CS | Idle HIGH, SPI2 chip select | SPI |
| 30 | PB11 | DRV2\_CS | GPIO\_Output | DRV2\_CS | Idle HIGH, driver 2 CS | SPI |
| 33 | PB12 | DRV1\_CS | GPIO\_Output | DRV1\_CS | Idle HIGH, driver 1 CS | SPI |
| 34 | PB13 | SPI2\_SCK | SPI2\_SCK | SPI2\_SCK | Shared: 2 drivers + IMU | SPI |
| 35 | PB14 | SPI2\_MISO | SPI2\_MISO | SPI2\_MISO | Shared bus, 10k pullup (bodge) | SPI |
| 36 | PB15 | SPI2\_MOSI | SPI2\_MOSI | SPI2\_MOSI | Shared bus | SPI |
| 37 | PC6 | M2\_INHA | TIM8\_CH1 | M2\_INHA | Center-aligned PWM, complementary off | MOTOR |
| 38 | PC7 | M2\_INHB | TIM8\_CH2 | M2\_INHB | PWM | MOTOR |
| 39 | PC8 | M2\_INHC | TIM8\_CH3 | M2\_INHC | PWM | MOTOR |
| 40 | PC9 | NFAULT | GPIO\_Input / EXTI | NFAULT | Pull-up 10k to 3V3 (bodge), open-drain in | CTRL |
| 41 | PA8 | M1\_INHA | TIM1\_CH1 | M1\_INHA | Center-aligned PWM, TIM1 master | MOTOR |
| 42 | PA9 | M1\_INHB | TIM1\_CH2 | M1\_INHB | PWM | MOTOR |
| 43 | PA10 | M1\_INHC | TIM1\_CH3 | M1\_INHC | PWM | MOTOR |
| 44 | PA11 | IMU\_INT | GPIO\_EXTI | IMU\_INT | Data-ready interrupt input | SPI |
| 45 | PA12 | ENC2\_CS | GPIO\_Output | ENC2\_CS | Idle HIGH, encoder 2 CS | SPI |
| 46 | PA13 | SWDIO | SYS\_JTMS-SWDIO | SWDIO | Debug = Serial Wire | DBG |
| 49 | PA14 | SWCLK | SYS\_JTCK-SWCLK | SWCLK | Debug = Serial Wire | DBG |
| 50 | PA15 | ENC1\_CS | GPIO\_Output | ENC1\_CS | Idle HIGH, encoder 1 CS | SPI |
| 51 | PC10 | LED\_RED | GPIO\_Output | LED\_RED | Push-pull out, verify actual color | IO |
| 52 | PC11 | LED\_GRN | GPIO\_Output | LED\_GRN | Push-pull out | IO |
| 53 | PC12 | LED\_BLUE | GPIO\_Output | LED\_BLU | Push-pull out | IO |
| 2 | PC13 | NSLEEP | GPIO\_Output | NSLEEP | Drive HIGH early (wake driver); low-drive pin | CTRL |
| 3 | PC14 | DRVOFF | GPIO\_Output | DRVOFF | Drive safe-state early; low-drive pin | CTRL |
| 4 | PC15 | (spare) | GPIO / unused | SPARE\_PC15 | No-connect / spare | IO |
| 5 | PH0 | BTN\_START | GPIO\_Input | BTN\_START | Internal pull-up, HSE off (GPIO mode) | IO |
| 6 | PH1 | BTN\_MODE | GPIO\_Input | BTN\_MODE | Internal pull-up, HSE off | IO |
| 55 | PB3 | IR\_EM1 | GPIO\_Output | IR\_EM1 | JTAG pin - set SWD-only; gate drive | IR |
| 56 | PB4 | IR\_EM2 | GPIO\_Output | IR\_EM2 | JTAG pin - SWD-only; gate drive | IR |
| 57 | PB5 | IR\_EM3 | GPIO\_Output | IR\_EM3 | Gate drive pulse | IR |
| 58 | PB6 | IR\_EM4 | GPIO\_Output | IR\_EM4 | Gate drive pulse | IR |
| 59 | PB7 | IR\_EM5 | GPIO\_Output | IR\_EM5 | Gate drive pulse | IR |
| 60 | BOOT0 | BOOT0 | (boot pin) | BOOT0 | 10k pulldown to GND, boot from flash | PWR |
| 61 | PB8 | TAIL\_PWM | TIMx\_CHx or GPIO | TAIL\_PWM | PWM to tail-light MOSFET gate | IO |
| 62 | PB9 | BUZZ\_PWM | TIMx\_CHx | BUZZ\_PWM | PWM to buzzer MOSFET gate | IO |
| 54 | PD2 | IR\_EM6 | GPIO\_Output | IR\_EM6 | Gate drive pulse (spare-capable) | IR |
| 1 | VBAT | VBAT->3V3 | (supply) | VBAT\_MCU | Tied to 3V3 + 100nF (no coin cell) | PWR |
| 7 | NRST | NRST | (reset) | NRST | 100nF to GND, to SWD header | PWR |
| 13 | VDDA | VDDA | (supply) | VDDA | 3V3 via ferrite bead + 1uF/100nF | PWR |
| 12 | VSSA | VSSA | (ground) | VSSA | To GND plane | PWR |
| 31 | VCAP\_1 | VCAP\_1 | (internal reg) | VCAP1 | 2.2uF at pin, no trace to VCAP\_2 | PWR |
| 47 | VCAP\_2 | VCAP\_2 | (internal reg) | VCAP2 | 2.2uF at pin | PWR |
| 19/32/48/64 | VDD | VDD | (supply) | VDD | 100nF each pin + 4.7uF bulk | PWR |
| 18/63 | VSS | VSS | (ground) | VSS | To GND plane | PWR |

## Clock & Peripheral Setup
| Setting | Value / Note |
| --- | --- |
| SYSTEM CLOCK | NaN |
| Source | HSI (16 MHz internal) - no external crystal |
| PLL | HSI -> PLL -> 168 MHz SYSCLK |
| PLL config | M=8, N=168, P=2 (=168MHz), Q=7 (48MHz for USB if needed) |
| AHB | 168 MHz |
| APB1 | 42 MHz (timers x2 = 84MHz) |
| APB2 | 84 MHz (timers x2 = 168MHz) |
| Debug | Serial Wire (SYS -> Debug = Serial Wire) |
| NaN | NaN |
| TIMERS - MOTOR PWM | NaN |
| TIM1 | Motor 1, center-aligned PWM, CH1/2/3 = PA8/9/10, MASTER (TRGO update) |
| TIM8 | Motor 2, center-aligned PWM, CH1/2/3 = PC6/7/8, SLAVE (sync to TIM1 via ITR0) |
| PWM freq | ~20-30 kHz typical for FOC (set ARR accordingly) |
| Trigger | TIM1 TRGO triggers ADC injected conversion at counter bottom |
| NaN | NaN |
| ADC - CURRENT SENSE + SENSORS | NaN |
| ADC1 | Injected: M1\_ISENSE A/B/C (PC0/1/2). Regular: IR\_RX + VBAT\_SENSE |
| ADC2 | Injected: M2\_ISENSE A/B/C (PA0/1/2) |
| Injected trigger | TIM1 TRGO (M1), TIM8 TRGO (M2) - sample at PWM bottom |
| DMA | Regular conversions via DMA (circular) for IR/battery |
| NaN | NaN |
| SPI | NaN |
| SPI1 | Encoders (AS5047P x2). Master, Mode 1 (CPOL0/CPHA1), ~ up to 10MHz |
| SPI2 | Drivers (DRV8316 x2) + IMU (ICM-42670). Master. CS: PB12/PB11/PB10 |
| NaN | NaN |
| DRIVER INIT ORDER (firmware, first actions) | NaN |
| 1 | Drive NSLEEP (PC13) HIGH - wake driver |
| 2 | Drive DRVOFF (PC14) to safe state (HIGH=motors off until ready) |
| 3 | SPI2: DRV8316 config - BUCK\_DIS=1 (disable internal buck) |
| 4 | SPI2: DRV8316 PWM\_MODE = 3x (Ctrl Reg2 bits[2:1]=10b) |
| 5 | Tie/confirm INLA/B/C = AVDD (hardware) for 3x mode |
| 6 | Read nFAULT / fault registers - confirm no fault, not in test mode |
| 7 | Then enable PWM, run encoder alignment, start FOC |
| NaN | NaN |
| GPIO NOTES | NaN |
| PC13/PC14 | Plain GPIO - NOT LSE/RTC. Low-drive pins, OK for static control. |
| PH0/PH1 | GPIO buttons - requires HSE disabled (no external crystal, so fine) |
| PB3/PB4 | JTAG pins - set Debug=Serial Wire to free them as GPIO (IR\_EM1/2) |
| PB2 | BOOT1 - free GPIO or no-connect |
| BOOT0 | 10k pulldown - boots from flash |