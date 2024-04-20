/**
  ******************************************************************************
  * @file    ads.c
  * @author  Stephen Taylor
  * @brief   Soil Power Sensor ADS12 library
  *          This file provides a function to read from the onboard ADC (ADS1219).
  * @date    11/27/2023
  *
  ******************************************************************************
  **/

/* Includes ------------------------------------------------------------------*/
#include "ads.h"

int HAL_status(HAL_StatusTypeDef ret) {
  int status;
  if (ret == HAL_OK){
    status = 0;
  } else if (ret == HAL_ERROR){
    status = 1;
  } else if (ret == HAL_BUSY){
    status = 2;
  } else {
    status = 3;
  }
  return status;
}

/**
******************************************************************************
* @brief    This function starts up the ADS1219
* 
*           This function is a wrapper for the STM32 HAl I2C library. The ADS1219 uses
*           I2C the I2C communication protocol. This function configures then ADS1219 for
*           single read mode. Note: the ADS1219 requires a minimum of 500us when it is powered on.
*
* @param    void
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef ADC_init(void){
  uint8_t code = ADS12_RESET_CODE;
  uint8_t register_data[2] = {0x40, 0x03};
  HAL_StatusTypeDef ret;

  // Control register breakdown.
  //  7:5 MUX (default)
  //  4   Gain (default)
  //  3:2 Data rate (default)
  //  1   Conversion mode (default)
  //  0   VREF (External reference 3.3V)


  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); // Power down pin has to be set to high before any of the analog circuitry can function
  //HAL_Delay(1000);
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);  // Send the reset code
  if (ret != HAL_OK){
    return ret;
  } 

  // Set the control register, leaving everything at default except for the VREF, which will be set to external reference mode
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, register_data, 2, HAL_MAX_DELAY);
  if (ret != HAL_OK){
    return ret;
  }
  
  code = ADS12_START_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY); // Send a start code
  if (ret != HAL_OK){
    return ret;
  }
  return ret;
}

/**
******************************************************************************
* @brief    This function reads the current ADC value.
* 
*           This function is a wrapper for the STM32 HAl I2C library. The ADS1219 uses
*           I2C the I2C communication protocol. 
*           
* @param    void
* @return   float, current ADC reading
******************************************************************************
*/
int ADC_readVoltage(void){
  uint8_t code;
  uint32_t reading;
  HAL_StatusTypeDef ret;
  uint8_t rx_data[3] = {0x00, 0x00, 0x00}; // Why is this only 3 bytes?
  
  while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))); // Wait for the DRDY pin on the ADS12 to go low, this means data is ready
  code = ADS12_READ_DATA_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK){
    return ret;
  }

  ret = HAL_I2C_Master_Receive(&hi2c2, ADS12_READ, rx_data, 3, 1000);
  if (ret != HAL_OK){// Recieve the ADS data from
    return ret;
  }
  

  reading = ((int)rx_data[0] << 8) | (int)rx_data[1]; // Chop the last byte, as it seems to be mostly noise

  int calibrated;
  calibrated = (VOLTAGE_SLOPE * ((int) reading)) + VOLTAGE_B; // Calculated from linear regression
  return calibrated;
}

/**
******************************************************************************
* @brief    This function probes the ADS12 to see if it is responsive.
*           
* @param    void
* @return   HAL_StatusTypeDef
******************************************************************************
*/
HAL_StatusTypeDef probeADS12(void){
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_IsDeviceReady(&hi2c2, ADS12_WRITE, 10, 20);
  return ret;
}

int ADC_filter(int readings[], int size){
  int filtered_reading = 0;
  for(int i = 0; i < size; i++){
    filtered_reading = filtered_reading + readings[i];
  }
  filtered_reading = filtered_reading / size;
  return filtered_reading;
}

size_t ADC_measure(uint8_t *data) {
  // get timestamp
  SysTime_t ts = SysTimeGet();

  // read voltage
  int adc_voltage = ADC_readVoltage();
  double adc_voltage_float = ((double) adc_voltage) / 1000.;

  // encode measurement
  size_t data_len = EncodePowerMeasurement(ts.Seconds, LOGGER_ID, CELL_ID,
                                           adc_voltage_float, 0.0, data);

  // return number of bytes in serialized measurement 
  return data_len;
}