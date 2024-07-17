/**
 ******************************************************************************
 * @file     I2Ccom.h
 * @author   Varun Sreedharan
 * @brief    This file allows communication between the ESP32 (secondary device) and STM32 (primary)
 *
 * @date     7/16/24
 ******************************************************************************
 */

#ifndef ESP32I2C_H
#define ESP32I2C_H

#include "main.h"
#include "usart.h"
#include "i2c.h"
#include "stm32_timer.h"

typedef struct {
    uint8_t *data;
    size_t size;
} I2CData;

// Global variable to hold the data
extern I2CData g_I2CData;
extern uint32_t counter;

/**
******************************************************************************
* @brief    Update the data to be transmitted.
*
* This function formats the message as "hello [i]" where `i` is a counter 
* that increments with each call.
*
* @param    void
* @return   void
******************************************************************************
*/
void updateData(void);

/**
******************************************************************************
* @brief    Transmit data over I2C.
*
* This function transmits the data stored in `g_I2CData` over I2C to a 
* specified address. It also handles the reception of an acknowledgment (ACK).
*
* @param    void
* @return   void
******************************************************************************
*/
void TransmitI2C(void);

/**
******************************************************************************
* @brief    Schedule the I2C transmission task.
*
* This function schedules the I2C transmission task by updating the data to be 
* transmitted and setting the appropriate task in the sequencer.
*
* @param    void *context
* @return   void
******************************************************************************
*/
void onTransmitI2C(void *context);

/**
******************************************************************************
* @brief    Receive the acknowledgment (ACK) from the I2C master.
*
* This function handles the reception of an acknowledgment (ACK) from the I2C 
* master and updates the state based on the reception status.
*
* @param    void
* @return   void
******************************************************************************
*/
void ReceiveACK(void);

#endif // ESP32I2C_H
