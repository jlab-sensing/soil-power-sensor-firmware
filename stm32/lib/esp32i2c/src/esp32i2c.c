/**
 ******************************************************************************
 * @file    esp32i2c.c
 * @brief   This file allows for communication between ESP32 and STM32
 * @date    4/10/2024
 *
 ******************************************************************************
 **/

#include "../include/esp32i2c.h"

// Global variables
I2CData g_I2CData;
uint32_t counter = 1;  // Counter for the hello message, starting at 1

void updateData(void) {
    // Format the message as "hello [i]"
    static char message[20];
    sprintf(message, "hello %lu", counter++);
    g_I2CData.data = (uint8_t *)message;
    g_I2CData.size = strlen(message) + 1;
}

void TransmitI2C(void) {
    if (g_I2CData.data == NULL || g_I2CData.size == 0) {
        // Log error or handle the situation when no data is set
        char error_str[] = "No data to transmit\n";
        HAL_UART_Transmit(&huart1, (uint8_t*)error_str, strlen(error_str), 1000);
        return;
    }

    uint8_t address = 0x6B << 1;  // I2C address, shifted for write operation
    char output[100];
    int output_len;

    // Log data to be transmitted
    output_len = sprintf(output, "Transmitting data over I2C: %s\n", g_I2CData.data);
    HAL_UART_Transmit(&huart1, (uint8_t*)output, output_len, 1000);

    // Transmit the data over I2C
    HAL_StatusTypeDef result = HAL_I2C_Master_Transmit(&hi2c2, address, g_I2CData.data, g_I2CData.size, 1000);
    if (result == HAL_OK) {
        output_len = sprintf(output, "I2C Transmission successful.\n");
    } else {
        output_len = sprintf(output, "I2C Transmission failed, HAL status: %d\n", result);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)output, output_len, 1000);
}

void ReceiveACK(void) {
    uint8_t address = 0x6B << 1;  // I2C address, shifted for write operation
    char output[100];
    int output_len;

    // Handle ACK reception
    uint8_t ackBuffer[16]; // Buffer to receive ACK, increased size for longer messages
    HAL_StatusTypeDef result = HAL_I2C_Master_Receive(&hi2c2, address, ackBuffer, sizeof(ackBuffer), 1000);
    if (result == HAL_OK) {
        if (strncmp((char*)ackBuffer, "ACK", 3) == 0) {
            output_len = sprintf(output, "ACK received.\n");
        } else if (strncmp((char*)ackBuffer, "STORE_LOCALLY", 13) == 0) {
            // Instead of storing locally, print a message
            output_len = sprintf(output, "Received STORE_LOCALLY ack, storing data locally (simulated).\n");
        } else {
            output_len = sprintf(output, "Received unexpected ACK: %s\n", ackBuffer);
        }
    } else {
        output_len = sprintf(output, "No ACK received, HAL status: %d\n", result);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)output, output_len, 1000);
}

void onTransmitI2C(void *context) {
    // Log task scheduling
    char task_str[] = "Scheduling I2C transmission task\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)task_str, strlen(task_str), 1000);

    // Schedule the state machine to run
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_TransmitI2C), CFG_SEQ_Prio_0);
}