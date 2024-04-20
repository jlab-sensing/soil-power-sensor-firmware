/**
 ******************************************************************************
 * @file     sdi12.h
 * @author   Stephen Taylor
 * @brief    This file contains all the driver functions for the sdi12 interface
 *
 * @date     1/30/2024
 ******************************************************************************
 */

#ifndef __SDI12_H__
#define __SDI12_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>

#include "i2c.h"
#include "sys_app.h"
#include "stm32_systime.h"

#include "bitbang.h"
#include "user_config.h"

#define WAKE_SENSOR_DELAY 13
#define MARKING_DELAY 9
#define MAX_RESPONSE_SIZE 100
#define ACTIVE_AWCKNOWLEDGE_COMMAND_SIZE 2
#define START_MEASUREMENT_COMMAND_SIZE 3
#define START_MEASURMENT_RESPONSE_SIZE 7
#define SEND_DATA_COMMAND_SIZE 4
#define SEND_DATA_RESPONSE_SIZE 16 // You need to add the numvalues (n) value returned from start measurment
#define SECONDS_TO_MILLISECONDS 1000
#define SERVICE_REQUEST_SIZE 3
#define SCALE_VWC_TO_INT 100
#define SCALE_TMP_TO_INT 10

typedef struct
{
  uint32_t Pin;
  GPIO_TypeDef *Port;
} SDI12_TypeDef;

typedef struct
{
  char Address;
  uint16_t Time;
  uint8_t NumValues;
} SDI12_Measure_TypeDef;

typedef struct
{
  int ec;
  float vwc_raw;
  float vwc_adj;
  float tmp;
  int addr;
} Teros12_Data;


/**
******************************************************************************
* @brief    This is a initialization for the SDI-12 data line.
*
* @param    GPIO_TypeDef *GPIOx, an instance of the typdef struct GPIO_Typedef
* @param    uint16_t, GPIO_Pin
* @return   void
******************************************************************************
*/
void SDI12_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/**
******************************************************************************
* @brief    Wake all sensors on the data line.
*
* @param    void
* @return   void
******************************************************************************
*/
void SDI12_WakeSensors(void);

/**
******************************************************************************
* @brief    Send a command via SDI12
*
* @param    const char *, command
* @return   void
******************************************************************************
*/
void SDI12_SendCommand(const char *command, uint8_t size);

/**
******************************************************************************
* @brief    Read data from a sensor via SDI-12
*
* @param    char *, buffer
* @param    uint16_t, bufferSize
* @param    uint16_t, timeoutMillis
* @param    const char *, command
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef SDI12_ReadData(char *buffer, uint16_t bufferSize, uint16_t timeoutMillis);

/**
******************************************************************************
* @brief    This is a function to read a measurment from a particular sensor.
*
* @param    char const addr, the device address
* @param    SDI12_Measure_TypeDef, a custom struct to store the measurment information returned from start measurment
* @param    char* the measurment data returned
* @param    uint16_t timeoutMillis time out in milliseconds
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef SDI12_GetMeasurment(uint8_t addr, SDI12_Measure_TypeDef *measurment_info, char *measurment_data, uint16_t timeoutMillis);

/**
******************************************************************************
* @brief    This is a function to get a measurment from the Teros12
*
* @param    const char addr, the address of the sensor
* @param    Teros12_data, teros_readings the teros readings
* @param    uint16_t timeoutMillis, the timeout in milliseconds
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef SDI12_GetTeros12Measurement(uint8_t addr, Teros12_Data *teros_readings, uint16_t timeoutMillis);

/**
******************************************************************************
* @brief    This is a function to parse a measurement from the returned SDI12 buffer
*           You'll notice is incomplete, that is because at the time of writing (2/21/24)
*           the group decided to go in a different design direction, so Stephen Taylor left 
*           it incomplete.
*
* @param    const char addr, the address of the sensor
* @param    Teros12_data, teros_readings the teros readings
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef SDI12_ParseTeros12Measurement(const char *buffer, Teros12_Data *teros_readings);

/**
******************************************************************************
* @brief    This is a function to ping a certain device, and see if it's active
*
* @param    char const addr, the device address

* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef SDI12_PingDevice(uint8_t deviceAddress, char *responseBuffer, uint16_t bufferSize, uint32_t timeoutMillis);

/**
 * @brief Reads TEROS21 and serializes measurement
 * 
 * The TEROS12 is measured over SDI-12. The volumetric water content
 * measurement has a calibration applied based on the soil type to convert
 * the raw reading into a percentage. Resulted sensor readings are serialized
 * using protobuf Teros12Measurement.
 * 
 * @note Implemented for the sensors library
 * 
 * @see SensorsPrototypeMeasure
 */
size_t SDI12_Teros12Measure(uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* __SDI12_H__ */