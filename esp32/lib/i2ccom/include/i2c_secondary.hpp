/**
 ******************************************************************************
 * @file     i2c_secondary.hpp
 * @author   Varun Sreedharan
 * @brief    This file contains all the driver functions for i2c secondary communication with
 *           STM32 board
 *
 * @date     5/1/2024
 ******************************************************************************
 */
#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP
#define MAX_BUFFER_SIZE 242

#include <Arduino.h>
#include <Wire.h>

// Define error codes for I2C operations
typedef enum {
    I2C_OK = 0,                // No error, operation successful
    I2C_ERROR_BUFFER_OVERFLOW, // Error code for buffer overflow
    I2C_ERROR_UNKNOWN          // Error code for an unspecified error
} i2c_error_t;

/**
******************************************************************************
* @brief    Initializes the SDI-12 data line.
*           This function configures the GPIO pin used for the SDI-12 data line.
* 
* @return   void
******************************************************************************
*/
void initI2C();

/**
******************************************************************************
* @brief    Handles the request to start communication on the I2C bus.
*           This function prepares the I2C interface to begin transmission or reception.
*
* @return   void
******************************************************************************
*/
void onRequest();

/**
******************************************************************************
* @brief    Handles the reception of data over the I2C bus.
*           This function processes the incoming data once a reception event occurs.
* 
* @param    int len: Number of bytes expected to receive.
* @return   void
******************************************************************************
*/
void onReceive(int len);

/**
******************************************************************************
* @brief    Resets the data buffer used in I2C communications.
*           This function clears or reinitializes the buffer to prepare for new data.
*
* @return   void
******************************************************************************
*/
void resetDataBuffer();

/**
******************************************************************************
* @brief    Returns a pointer to the current data buffer.
*           This function provides access to the data buffer holding the received I2C data.
*
* @return   uint8_t*: Pointer to the data buffer.
******************************************************************************
*/
uint8_t* returnDataBuffer();

/**
******************************************************************************
* @brief    Retrieves the count of data bytes in the I2C buffer.
*           This function returns the number of bytes currently stored in the data buffer.
*
* @return   int: Number of data bytes in the buffer.
******************************************************************************
*/
int getDataCount();

/**
******************************************************************************
* @brief    Retrieves the last error state of the I2C communication.
*           This function returns the last error code set by I2C operations.
*
* @return   i2c_error_t: Error code representing the last I2C error.
******************************************************************************
*/
i2c_error_t getI2CError();

/**
******************************************************************************
* @brief    Clears the last error state of the I2C communication.
*           This function resets the error state to I2C_OK, indicating no error.
*
* @return   void
******************************************************************************
*/
void clearI2CError();

#endif // I2C_SLAVE_HPP
