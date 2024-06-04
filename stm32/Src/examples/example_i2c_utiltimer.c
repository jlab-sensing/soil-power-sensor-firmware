#include "esp32i2c.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "rtc.h"
#include "platform.h"
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "adc_if.h"
#include "CayenneLpp.h"
#include "sys_sensors.h"
#include "flash_if.h"
#include <stdio.h>

typedef struct {
    uint8_t *data;
    size_t size;
} I2CData;

// Global variable to hold the data
I2CData g_I2CData;

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

    // Handle ACK reception
    uint8_t ackBuffer[3]; // Buffer to receive ACK
    result = HAL_I2C_Master_Receive(&hi2c2, address, ackBuffer, sizeof(ackBuffer), 1000);
    if (result == HAL_OK && strncmp((char*)ackBuffer, "ACK", 3) == 0) {
        output_len = sprintf(output, "ACK received.\n");
    } else {
        output_len = sprintf(output, "\nNo ACK received, HAL status: %d\n", result);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)output, output_len, 1000);
}

void onTransmitI2C(void *context) {
    // Log task scheduling
    char task_str[] = "Scheduling I2C transmission task\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)task_str, strlen(task_str), 1000);
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_TransmitI2C), CFG_SEQ_Prio_0);
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure LSE Drive Capability */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Configure the main internal regulator output voltage */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3 | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_I2C2_Init();
    SystemApp_Init();

    // Initialize data to be transmitted
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '\0'};
    g_I2CData.data = data;
    g_I2CData.size = sizeof(data);

    // Send UART message
    char info_str[100];
    sprintf(info_str, "I2C Master Controller initialized, compiled on %s %s\n", __DATE__, __TIME__);
    HAL_UART_Transmit(&huart1, (uint8_t*)info_str, strlen(info_str), 1000);

    // Register I2C transmission task
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_TransmitI2C), UTIL_SEQ_RFU, TransmitI2C);

    // Create and start a timer for I2C transmission
    UTIL_TIMER_Object_t i2cTimer;
    UTIL_TIMER_Create(&i2cTimer, 1000, UTIL_TIMER_PERIODIC, onTransmitI2C, NULL);
    UTIL_TIMER_Start(&i2cTimer);

    while (1) {
        UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
