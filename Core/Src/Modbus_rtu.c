/*
 * Modbus_rtu.c
 *
 *  Created on: Aug 7, 2026
 *      Author: Safwan
 */

/*
 * modbus_rtu.c
 *
 * Implements CRC16 (Modbus flavor), frame building, and DE/RE-timed
 * transmit/receive - the same things minimalmodbus was doing for us
 * invisibly on the PC side.
 */

#include "modbus_rtu.h"
#include <string.h>

static UART_HandleTypeDef *modbus_huart;

#define MODBUS_TIMEOUT_MS   500

void Modbus_Init(UART_HandleTypeDef *huart)
{
    modbus_huart = huart;
    /* Start in receive mode */
    HAL_GPIO_WritePin(MODBUS_DE_RE_GPIO_PORT, MODBUS_DE_RE_GPIO_PIN, GPIO_PIN_RESET);
}

/* Standard Modbus RTU CRC16 (poly 0xA001, init 0xFFFF) */
static uint16_t Modbus_CRC16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}


/* Sends a frame, then switches to receive and waits for a response.
 * expected_len is the total frame length we expect back (including CRC). */
static ModbusStatus Modbus_Transact(uint8_t *tx_frame, uint16_t tx_len,
                                     uint8_t *rx_frame, uint16_t expected_len)
{
    /* --- Transmit --- */
    HAL_GPIO_WritePin(MODBUS_DE_RE_GPIO_PORT, MODBUS_DE_RE_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay(1);  /* small settle time before driving the bus */

    HAL_UART_Transmit(modbus_huart, tx_frame, tx_len, MODBUS_TIMEOUT_MS);

    /* Wait for transmission to physically finish before releasing the bus */
    while (__HAL_UART_GET_FLAG(modbus_huart, UART_FLAG_TC) == RESET) { }

    HAL_GPIO_WritePin(MODBUS_DE_RE_GPIO_PORT, MODBUS_DE_RE_GPIO_PIN, GPIO_PIN_RESET);

    /* --- Receive --- */
    HAL_StatusTypeDef status = HAL_UART_Receive(modbus_huart, rx_frame,
                                                 expected_len, MODBUS_TIMEOUT_MS);
    if (status != HAL_OK) {
        return MODBUS_ERR_TIMEOUT;
    }

    /* --- Verify CRC --- */
    uint16_t received_crc = rx_frame[expected_len - 2] |
                             (rx_frame[expected_len - 1] << 8);
    uint16_t calculated_crc = Modbus_CRC16(rx_frame, expected_len - 2);
    if (received_crc != calculated_crc) {
        return MODBUS_ERR_CRC;
    }

    return MODBUS_OK;
}

ModbusStatus Modbus_ReadRegister(uint16_t address, uint16_t *out_value)
{
    uint8_t tx_frame[8];
    uint8_t rx_frame[7];  /* slave(1) + func(1) + bytecount(1) + data(2) + crc(2) */

    tx_frame[0] = MODBUS_SLAVE_ADDRESS;
    tx_frame[1] = 0x03;                    /* function code: read holding register */
    tx_frame[2] = (address >> 8) & 0xFF;
    tx_frame[3] = address & 0xFF;
    tx_frame[4] = 0x00;                    /* number of registers, high byte */
    tx_frame[5] = 0x01;                    /* number of registers = 1 */
    uint16_t crc = Modbus_CRC16(tx_frame, 6);
    tx_frame[6] = crc & 0xFF;
    tx_frame[7] = (crc >> 8) & 0xFF;

    ModbusStatus result = Modbus_Transact(tx_frame, 8, rx_frame, 7);
    if (result != MODBUS_OK) {
        return result;
    }

    if (rx_frame[0] != MODBUS_SLAVE_ADDRESS || rx_frame[1] != 0x03) {
        return MODBUS_ERR_BAD_RESPONSE;
    }

    *out_value = (rx_frame[3] << 8) | rx_frame[4];
    return MODBUS_OK;
}

ModbusStatus Modbus_WriteRegister(uint16_t address, uint16_t value)
{
    uint8_t tx_frame[8];
    uint8_t rx_frame[8];  /* write single register echoes the same 8-byte frame */

    tx_frame[0] = MODBUS_SLAVE_ADDRESS;
    tx_frame[1] = 0x06;                    /* function code: write single register */
    tx_frame[2] = (address >> 8) & 0xFF;
    tx_frame[3] = address & 0xFF;
    tx_frame[4] = (value >> 8) & 0xFF;
    tx_frame[5] = value & 0xFF;
    uint16_t crc = Modbus_CRC16(tx_frame, 6);
    tx_frame[6] = crc & 0xFF;
    tx_frame[7] = (crc >> 8) & 0xFF;

    ModbusStatus result = Modbus_Transact(tx_frame, 8, rx_frame, 8);
    if (result != MODBUS_OK) {
        return result;
    }

    if (memcmp(tx_frame, rx_frame, 6) != 0) {
        return MODBUS_ERR_BAD_RESPONSE;
    }

    return MODBUS_OK;
}

ModbusStatus Modbus_WriteRegister_Retry(uint16_t address, uint16_t value)
{
    ModbusStatus result;
    for (int attempt = 1; attempt <= 3; attempt++) {
        result = Modbus_WriteRegister(address, value);
        if (result == MODBUS_OK) {
            return MODBUS_OK;
        }
        HAL_Delay(150);
    }
    return result;
}
