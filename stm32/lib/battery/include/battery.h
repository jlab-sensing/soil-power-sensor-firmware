/**
 * Copyright 2024 jLab
 * @file battery.h
 * @brief Provides declarations for battery monitoring interface.
 * 
 * Assumes the HAL layer is already initialized. The function, @ref battery_init calibrates the ADC and starts continuous conversion to a variable. The most recent battery voltage level is retrieved with @ref battery_voltage.
 * 
 * @author John Madden <jmadden173@pm.me>
 * @date 2023-11-16
*/

#ifndef STM32_LIB_BATTERY_INCLUDE_BATTERY_H_
#define STM32_LIB_BATTERY_INCLUDE_BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the battery monitoring module.
 * 
 * Starts continuous sampling on ADC.
 * 
 * @return Return code
*/
HAL_StatusTypeDef battery_init(void);

/**
 * @brief Gets the most recent battery voltage level
 * 
 * The battery voltage level is in mV.
 * 
 * @return Battery voltage
*/
unsigned int battery_voltage(void);

#ifdef __cplusplus
}
#endif

#endif  // STM32_LIB_BATTERY_INCLUDE_BATTERY_H_
