/**
 ******************************************************************************
 * @file     I2Ccom.h
 * @author   Varun Sreedharan
 * @brief    This file allows communication between the ESP32 (slave device) and STM32 (master)
 *
 * @date     4/10/24
 ******************************************************************************
 */

#ifndef I2C_COM_H
#define I2C_COM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wlxx_hal.h"

/**
 * @brief Transmits data over I2C to a specified slave device.
 * 
 * This function is responsible for initiating an I2C transmission to a slave device 
 * with a given I2C address. It sends a series of bytes stored in a buffer.
 * 
 * @param addr The 7-bit I2C address of the slave device.
 * @param data Pointer to the data buffer containing bytes to send.
 * @param size Number of bytes to transmit.
 * @return HAL_StatusTypeDef Returns HAL status (HAL_OK if successful).
 */
HAL_StatusTypeDef I2C2_Transmit(uint8_t addr, uint8_t* data, uint16_t size);

/**
 * @brief Receives data over I2C from a specified slave device.
 * 
 * This function is designed to receive a sequence of bytes from a slave device 
 * into a provided buffer. The function handles the complete receive process including 
 * waiting for the I2C bus to become free, if necessary.
 * 
 * @param addr The 7-bit I2C address of the slave device.
 * @param data Pointer to the buffer that will store received data.
 * @param size Number of bytes to receive.
 * @return HAL_StatusTypeDef Returns HAL status (HAL_OK if successful).
 */
HAL_StatusTypeDef I2C2_Receive(uint8_t addr, uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* I2C_COM_H */
