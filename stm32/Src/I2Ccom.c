/**
 ******************************************************************************
 * @file    I2Ccom.c
 * @author  Varun Sreedharan
 * @brief   This file allows for communication between ESP32 and STm32
 * @date    4/10/2024
 *
 ******************************************************************************
 **/

#include "I2Ccom.h"

extern I2C_HandleTypeDef hi2c2;  // Ensure hi2c2 is initialized elsewhere in your project

HAL_StatusTypeDef I2C2_Transmit(uint8_t addr, uint8_t* data, uint16_t size) {
    uint8_t frame[size + 1];  // Create a new frame with size+1 to include the length byte
    frame[0] = size;          // First byte is the length of the data
    memcpy(&frame[1], data, size);  // Copy the original data into frame starting at index 1
    return HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)(addr << 1), frame, size + 1, 1000);
}
