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

HAL_StatusTypeDef I2C2_Transmit(uint8_t addr, uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* I2C_COM_H */