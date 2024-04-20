/**
 ******************************************************************************
 * @file    sdi12.c
 * @author  Stephen Taylor
 * @brief   This file provides a library to communicate between a STM32 MC
 *          and a SDI-12 device.
 * @date    1/30/2024
 *
 ******************************************************************************
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdi12.h"

SDI12_TypeDef sdi12;

// Function to configure GPIO as output
void configureAsOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Enable GPIOA clock
    
    // Configure GPIO pin as output
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Function to configure GPIO as input
void configureAsInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE(); // Enable GPIOA clock

    // Configure GPIO pin as input
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


HAL_StatusTypeDef ParseMeasurementResponse(const char *responseBuffer, char addr, SDI12_Measure_TypeDef *measurement_info)
{
  char responseAddr;
  sscanf(responseBuffer, "%1c%3hu%1hhu", &responseAddr, &(measurement_info->Time), &(measurement_info->NumValues)); // Parse the response and populate the structure
  measurement_info->Address = addr; // Update address
  // Check if the response address matches the expected address
  if (responseAddr == addr)
  {
    return HAL_OK; // Return success
  }
  else
  {
    return HAL_ERROR; // Return error - Address mismatch
  }
  return HAL_OK;
}

HAL_StatusTypeDef ParseServiceRequest(const char *requestBuffer, char addr)
{
  char expectedResponse[12];
  sprintf(expectedResponse, "%c\r\n", addr); // Construct the expected response ("a\r\n")
  // Parse the service request
  if (memcmp(requestBuffer, expectedResponse, 3) == 0) // Check if the response matches the expected Active Acknowledge response
  {
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

void SDI12_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  sdi12.Pin = GPIO_Pin; // Configure the pin
  sdi12.Port = GPIOx;

  HAL_GPIO_WritePin(sdi12.Port, sdi12.Pin, GPIO_PIN_RESET); // Set the data line low
}

void SDI12_WakeSensors(void)
{
  SendContinousHigh(sdi12.Port, sdi12.Pin, 20); // Set the data line low
  HAL_Delay(10); // Marking after break, see SDi-12 guide
}

void SDI12_SendCommand(const char *command, uint8_t size)
{
  for (uint8_t i = 0; i < size; ++i) {
    SendCharacter(sdi12.Port, sdi12.Pin, command[i]);
    simpleDelay(); // Send stop bit between charcters
  }
  return;
}

HAL_StatusTypeDef SDI12_ReadData(char *buffer, uint16_t bufferSize, uint16_t timeoutMillis)
{
  uint32_t startTime = HAL_GetTick(); // Get the current time
  uint8_t recv = 0;
  uint16_t index = 0;
  uint8_t mask = 0x7f;

  while ((HAL_GetTick() - startTime) < timeoutMillis) { // wait for the timeout to pass
    if (HAL_GPIO_ReadPin(sdi12.Port, sdi12.Pin) == GPIO_PIN_SET){ // if the start bit arrives
      simpleDelayCentering(); // center the sample
      // Read each of the 8 bits
      for (uint8_t i = 0x01; i; i <<= 1) {
        simpleDelay();
        uint8_t noti = ~i;
        if (HAL_GPIO_ReadPin(sdi12.Port, sdi12.Pin)) {
          recv |= i;
        } else {
          // else clause added to ensure function timing is ~balanced
          recv &= noti;
        }
      }
      simpleDelay(); // skip the stop bit
      recv = ~recv; // Invert the data
      recv = recv & mask; // Cut the parity bit, which will be the first bit

      // Store the received byte in the buffer
      buffer[index] = recv;
      

      // Check for the termination characters
      if (buffer[index - 1] == '\r' && buffer[index] == '\n') {
        buffer[index + 1] = '\0'; // Null terminate the buffer
        return HAL_OK; // Successfully received the message
      }

      // Check if the buffer is full
      if (index > bufferSize) {
        return HAL_ERROR; // Buffer overflow, message is too long
      }
      index++;
    }
  }
  return HAL_TIMEOUT;
}

HAL_StatusTypeDef SDI12_GetMeasurment(uint8_t addr, SDI12_Measure_TypeDef *measurment_info, char *measurement_data, uint16_t timeoutMillis)
{
  char command[MAX_RESPONSE_SIZE];        // Command to request measurement ("M1!\r\n" for example)
  char responseBuffer[MAX_RESPONSE_SIZE]; // Buffer to store the response    char responseAddr; // Address in sensor response
  char sendData[MAX_RESPONSE_SIZE];
  HAL_StatusTypeDef ret;
  //char error[15];
  configureAsOutput();
  SDI12_WakeSensors();

  // Construct the command to request measurement
  sprintf(command, "%cM!", addr);
  sprintf(sendData, "%cD0!", addr);
  

  // Send the command to request measurement
  SDI12_SendCommand(command, START_MEASUREMENT_COMMAND_SIZE);
  configureAsInput();

  // Read the response from the device
  ret = SDI12_ReadData(responseBuffer, START_MEASURMENT_RESPONSE_SIZE, timeoutMillis); // Will need to change the size to the size of the time till data ready function
  if (ret != HAL_OK)
  {
    return ret;
  }

  // Parse the response and populate the structure
  ret = ParseMeasurementResponse(responseBuffer, addr, measurment_info); // Will need to figure out what even is happening in here
  if (ret != HAL_OK)
  {
    //sprintf(error, "error %d", ret); // Why does this need to be here?
    return ret;
  }

  if (measurment_info->Time == 0)
  { // If data is ready now
    configureAsOutput();
    SDI12_SendCommand(sendData, SEND_DATA_COMMAND_SIZE); 
    configureAsInput();
    ret = SDI12_ReadData(measurement_data, SEND_DATA_RESPONSE_SIZE + measurment_info->NumValues, timeoutMillis); // double check the sizeof
    return ret;
  }

  int millisStart = HAL_GetTick();
  while ((HAL_GetTick() - millisStart) < (measurment_info->Time * SECONDS_TO_MILLISECONDS)) // Wait for ttt to elapse
  { 
    if (HAL_GPIO_ReadPin(sdi12.Port, sdi12.Pin) == GPIO_PIN_SET) // If there is any activity on the data line
    { 
      ret = SDI12_ReadData(responseBuffer, SERVICE_REQUEST_SIZE, timeoutMillis); // Read the data
      if (ret != HAL_OK)
      {
        return ret;
      }
      ret = ParseServiceRequest(responseBuffer, measurment_info->Address); // Will need to go through this function again
      if (ret != HAL_OK)
      {
        return ret;
      }
      configureAsOutput(); // Wake and tell the sensor to send the data
      SDI12_WakeSensors();
      SDI12_SendCommand(sendData, SEND_DATA_COMMAND_SIZE);
      configureAsInput();
      // ret = SDI12_ReadData(measurement_data, SEND_DATA_RESPONSE_SIZE, timeoutMillis); // read the data and return
      // return ret;
      SDI12_ReadData(measurement_data, SEND_DATA_RESPONSE_SIZE, timeoutMillis); // lol is this works with 3 minutes to spare
      return HAL_OK;
    }
  }

  configureAsOutput();
  SDI12_WakeSensors(); // If ttt does elapse wake the sensors
  SDI12_SendCommand(sendData, SEND_DATA_COMMAND_SIZE);
  configureAsInput();
  ret = SDI12_ReadData(measurement_data, SEND_DATA_RESPONSE_SIZE + measurment_info->NumValues, timeoutMillis); 
  return ret;
}

HAL_StatusTypeDef SDI12_GetTeros12Measurement(uint8_t addr, Teros12_Data *teros_readings, uint16_t timeoutMillis)
{
  SDI12_Measure_TypeDef measurment_info;
  HAL_StatusTypeDef ret;
  char measurement_data[25];

  ret = SDI12_GetMeasurment(addr, &measurment_info, measurement_data, timeoutMillis); // Read from the TEROS
  if (ret != HAL_OK){
    return ret;
  }
  sscanf(measurement_data, "%d+%f+%f+%d\r\n",&(teros_readings->addr), &(teros_readings->vwc_raw), &(teros_readings->tmp), &(teros_readings->ec));
  
  //y=0.000000000512018x^3-0.000003854251138x^2+0.009950433111633x-8.508168835940560 calibration eqn
 teros_readings->vwc_adj = (0.000000000512018 * teros_readings->vwc_raw * teros_readings->vwc_raw * teros_readings->vwc_raw) -
                           (0.000003854251138 * teros_readings->vwc_raw * teros_readings->vwc_raw) +
                           (0.009950433111633 * teros_readings->vwc_raw) - 8.508168835940560;
  return HAL_OK;
}

HAL_StatusTypeDef SDI12_ParseTeros12Measurement(const char *buffer, Teros12_Data *teros_readings)
{
    // Parse address
    teros_readings->addr = buffer[0] - '0'; // Convert char to int

    // Initialize variables
    int i = 2;  // Start of vwc_raw is always at the third character in the string
    char floatBuffer[20];  // Assuming a reasonable maximum length for the float representation

    // Parse vwc_raw
    while (buffer[i] != '+') {
        floatBuffer[i - 2] = buffer[i];
        i++;
    }
    floatBuffer[i - 2] = '\0';  // Null-terminate the substring
    teros_readings->vwc_raw = (int)(strtof(floatBuffer, NULL) * 100);

    // Reset floatBuffer
    memset(floatBuffer, 0, sizeof(floatBuffer));

    // Skip the '+'
    i++;

    // Parse tmp
    int tmp = 0;
    while (buffer[i] != '+') {
        floatBuffer[i - 2] = buffer[i];
        i++;
    }
    floatBuffer[i - 2] = '\0';  // Null-terminate the substring
    teros_readings->tmp = (int)(strtof(floatBuffer, NULL) * 100);

    // Reset floatBuffer
    memset(floatBuffer, 0, sizeof(floatBuffer));

    // Skip the '+'
    i++;
    // Parse ec
    int ec = 0;
    while (buffer[i] != '\0' && buffer[i] != '\r' && buffer[i] != '\n') {
        ec = ec * 10 + (buffer[i] - '0');
        i++;
    }
    teros_readings->ec = ec;
    return HAL_OK;
}

HAL_StatusTypeDef SDI12_PingDevice(uint8_t deviceAddress, char *responseBuffer, uint16_t bufferSize, uint32_t timeoutMillis)
{
  HAL_StatusTypeDef ret;
  char command[6];          // Active Acknowledge command with address and termination character
  char expectedResponse[5]; // Expected response for Active Acknowledge

  sprintf(command, "%c!", deviceAddress); // Construct the Active Acknowledge command (e.g., "a0!\r\n")
  sprintf(expectedResponse, "%c\r\n", deviceAddress); // Construct the expected response ("a\r\n")

  configureAsOutput(); // Set serial line to output
  SDI12_WakeSensors(); // Wake the sensors
  SDI12_SendCommand(command, ACTIVE_AWCKNOWLEDGE_COMMAND_SIZE); // Send the Active Acknowledge command
  configureAsInput(); // Set the serial line as input
  
  ret = SDI12_ReadData(responseBuffer, bufferSize, timeoutMillis); // Read the response from the device
  if (ret != HAL_OK)
  {
    return ret;
  }
  if (memcmp(responseBuffer, expectedResponse, bufferSize) == 0) // Check if the response matches the expected Active Acknowledge response
  {
    ret = HAL_OK;
  }
  else
  {
    ret = HAL_ERROR;
  }
  return ret;
}

size_t SDI12_Teros12Measure(uint8_t *data) {
  // address of teros 
  uint8_t teros_addr = '0';

  // structure to store teros measurement
  Teros12_Data teros_measurements;

  // backup data if wwe get a HAL error
  static Teros12_Data teros_backup;

  // get timestamp
  SysTime_t ts = SysTimeGet();

  // get teros measurement
  HAL_StatusTypeDef ret;
  ret = SDI12_GetTeros12Measurement(teros_addr, &teros_measurements, 1000);
  if (ret != HAL_OK) {
    simpleDelay();
    simpleDelay();
    simpleDelay();
    ret = SDI12_GetTeros12Measurement(teros_addr, &teros_measurements, 1000);
  }

  // check for negative vwc
  if (teros_measurements.vwc_adj < 0) {
    teros_measurements = teros_backup;
  }

  // check return codes
  APP_LOG(TS_ON, VLEVEL_M, "TEROS12 HAL ret: ");
  if (ret == HAL_OK) {
    APP_LOG(TS_OFF, VLEVEL_M, "HAL_OK\r\n");
    teros_backup = teros_measurements;
  }
  else if (ret == HAL_ERROR) {
    APP_LOG(TS_OFF, VLEVEL_M, "HAL_ERROR\r\n");
    teros_measurements = teros_backup;
  }
  else if (ret == HAL_TIMEOUT) {
    APP_LOG(TS_OFF, VLEVEL_M, "HAL_TIMEOUT\r\n");
    teros_measurements = teros_backup;
  }

  // serialize data
  size_t data_len = EncodeTeros12Measurement(ts.Seconds, LOGGER_ID, CELL_ID,
                                             (double)teros_measurements.vwc_raw,
                                             (double)teros_measurements.vwc_adj,
                                             (double)teros_measurements.tmp,
                                             teros_measurements.ec, data);

  // return number of bytes in serialized measurement
  return data_len;
}