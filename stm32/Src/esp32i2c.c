/**
 ******************************************************************************
 * @file    I2Ccom.c
 * @author  Varun Sreedharan
 * @brief   This file allows for communication between ESP32 and STm32
 * @date    4/10/2024
 *
 ******************************************************************************
 **/

#include "esp32i2c.h"

extern I2C_HandleTypeDef hi2c2;

HAL_StatusTypeDef I2C2_Transmit(uint8_t addr, uint8_t* data, uint16_t size) {
    // Adjust the frame size calculation
    uint8_t frame[size];  // Buffer to include command/size byte
    frame[0] = size - 1;  // The first byte should be the number of bytes that follow, not counting itself
    memcpy(&frame[1], data, size);  // Copy the actual data into the frame

    // Shift the 7-bit address left to form the 8-bit address (including the R/W bit)
    return HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)(addr << 1), frame, size + 1, 1000);
}