/*
 * Modbus_rtu.h
 *
 *  Created on: Aug 7, 2026
 *      Author: Safwan
 */

/*
 * modbus_rtu.h
 *
 * Minimal Modbus RTU master driver for STM32 HAL, talking to the
 * Leadshine EL7-RS400P over RS485 via a MAX3485 transceiver.
 *
 * Wiring:
 *   USART1 TX (PA9)  -> MAX3485 DI
 *   USART1 RX (PA10) -> MAX3485 RO
 *   DE/RE pin (PA8)  -> MAX3485 DE and /RE tied together
 *                       HIGH = transmit, LOW = receive
 *   MAX3485 A/B      -> RS485 bus (drive's 485+/485-)
 *
 * UART config (set in CubeMX): 115200 baud, 8 data bits, 2 stop bits,
 * no parity - matches the drive's P05.29/30/31 settings we already
 * confirmed working from the PC-based scripts.
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define MODBUS_DE_RE_GPIO_PORT   GPIOA
#define MODBUS_DE_RE_GPIO_PIN    GPIO_PIN_8

#define MODBUS_SLAVE_ADDRESS     0x01

typedef enum {
    MODBUS_OK = 0,
    MODBUS_ERR_TIMEOUT,
    MODBUS_ERR_CRC,
    MODBUS_ERR_BAD_RESPONSE,
} ModbusStatus;

/* Call once after HAL_UART_Init() on the handle you're using for RS485 */
void Modbus_Init(UART_HandleTypeDef *huart);

/* Read one holding register (function code 0x03).
 * out_value receives the register's value on success. */
ModbusStatus Modbus_ReadRegister(uint16_t address, uint16_t *out_value);

/* Write one holding register (function code 0x06). */
ModbusStatus Modbus_WriteRegister(uint16_t address, uint16_t value);

ModbusStatus Modbus_WriteRegister_Retry(uint16_t address, uint16_t value);

#endif /* MODBUS_RTU_H */
