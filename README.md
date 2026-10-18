# STM32_AC_SERVO_MODBUS

STM32 (Nucleo-F446RE) firmware that sets a control mode on a Leadshine **EL7-RS400P** servo drive and runs a test in that mode, over RS485 Modbus RTU. One drive, slave address 1.

## Modes

| Mode | Setup function (run once) | Run function |
|---|---|---|
| Velocity (P00.01 = 1) | `test_setup_velocity_mode()` | `test_velocity_mode()` |
| Torque (P00.01 = 2) | `test_setup_torque_mode()` | `test_torque_mode()` |
| PR, 16 paths (P00.01 = 6) | `test_setup_pr_mode()` | `test_pr_16_paths()` |

## How to use

Only **one** line is uncommented at a time in `main()`.

1. Uncomment one `test_setup_XXX_mode()` call, build, flash and run
2. **Power-cycle the drive** (not the Nucleo). The mode only takes effect after this
3. Comment the setup call out again
4. Uncomment the matching run function, build, flash and run

Each setup function writes P00.01 and saves it to EEPROM (P00.25 = `0x2211`). You only repeat steps 1-3 when you switch modes.

## What each test does

- **Velocity:** sets accel/decel, runs at 100 rpm for 5 s, ramps to 0, then disables the servo
- **Torque:** sets a 30 rpm velocity limit, steps through 5%, 10% and 20% torque (6 s each) so you can feel the difference by hand
- **PR:** configures 16 chained paths that loop continuously. The USER button stops it and returns to the 0-degree point

## Wiring

| STM32 | Connects to |
|---|---|
| PA9 / PA10 (USART1) | MAX3485 DI / RO |
| PA8 | MAX3485 DE + RE |
| PC13 | USER button (EXTI) |

MAX3485 A/B go to the drive's RS485+/RS485-, with a common ground. Serial settings: 115200, 8N2. Logs print on USART2 (ST-Link virtual COM port).

## Safety

Keep the loads light and the rpm low while testing, and keep your hand on the USER button. Servo enable is done over RS485 (P00.26 bit 4), so there is no physical enable wire.
