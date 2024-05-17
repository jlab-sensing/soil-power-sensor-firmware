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


void SystemClock_Config(void);

typedef struct {
    uint8_t *data;
    size_t size;
} I2CData;

// Global variable to hold the data
I2CData g_I2CData;

void TransmitI2C(void) {
    if (g_I2CData.data == NULL || g_I2CData.size == 0) {
        // Log error or handle the situation when no data is set
        return;
    }

    uint8_t address = 0x6B << 1;  // I2C address, shifted for write operation
    char output[100];
    int output_len;
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // Transmit the data over I2C
    HAL_StatusTypeDef result = I2C2_Transmit(address, data, sizeof(data));
    if (result == HAL_OK) {
        output_len = sprintf(output, "I2C Transmission successful.\n");
    } else {
        output_len = sprintf(output, "I2C Transmission failed, HAL status: %d\n", result);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)output, output_len, 1000);
}

void onTransmitI2C(void *context) {
    // As this function does not use the context parameter, you can ignore it
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_TransmitI2C), CFG_SEQ_Prio_0);
}

void Error_Handler(void) {
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
    /*Initialize timer and RTC*/
    //UTIL_TIMER_Init();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_I2C2_Init();
    SystemApp_Init();

    uint8_t address = 0x6B;  // I2C 7-bit address of the slave device

    char info_str[100];
    int info_len = sprintf(info_str, "I2C Master Controller initialized, compiled on %s %s\n", __DATE__, __TIME__);
    HAL_UART_Transmit(&huart1, (const uint8_t*)info_str, info_len, 1000);

    uint8_t data[] = {0xa, 0xa, 0x8, 0x4, 0x10, 0x7, 0x18, 0xf0, 0xab, 0xe3, 0xac,
                    0x5, 0x22, 0x12, 0x9, 0x14, 0xae, 0x47, 0xe1, 0x7a, 0x44,
                    0x96, 0x40, 0x11, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0xa8, 0x9e,
                    0x40};

    //uint8_t data[] = {'H', 'e', 'l', 'l', 'o', '!'}; // The first byte is the number of subsequent data bytes
    char output[100];
    int output_len;
    CFG_SEQ_Task_printHello;
    void onPrintHello(void) {
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_printHello), CFG_SEQ_Prio_0);
    }

    void printHello(void) {
      char hello_str[] = "hello\r\n";
      HAL_UART_Transmit(&huart1, (const uint8_t*)hello_str, sizeof(hello_str) - 1, 1000);
    }
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_printHello), UTIL_SEQ_RFU, printHello);
    UTIL_TIMER_Object_t testTimer;
    UTIL_TIMER_Create(&testTimer, 500, UTIL_TIMER_PERIODIC, onPrintHello, NULL);
    UTIL_TIMER_Start(&testTimer);
    while (1) {
        UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
        
        /*HAL_StatusTypeDef result = I2C2_Transmit(address, data, sizeof(data));

        if (result == HAL_OK) {
            output_len = sprintf(output, "I2C Transmission successful.\n");
        } else {
            output_len = sprintf(output, "I2C Transmission failed, HAL status: %d\n", result);
        }
        HAL_UART_Transmit(&huart1, (const uint8_t*)output, output_len, 1000);

        uint8_t* ackBuffer[3]; // Buffer to receive ACK
        result = HAL_I2C_Master_Receive(&hi2c2, (uint16_t)(address << 1), ackBuffer, sizeof(ackBuffer), 1000);
        if (result == HAL_OK && strncmp((char*)ackBuffer, "ACK", 3) == 0) {
            output_len = sprintf(output, "ACK received.\n");
        } else {
            output_len = sprintf(output, "\nNo ACK received, HAL status: %d\n", result);
        }
        HAL_UART_Transmit(&huart1, (const uint8_t*)output, output_len, 1000);
        HAL_Delay(1000);  // Delay between transmissions
        //output_len = sprintf(output, "I didn't crash\n");
        //HAL_UART_Transmit(&huart1, (const uint8_t*)output, output_len, 1000);*/
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  // char error[30];
  // int error_len = sprintf(error, "Error!  HAL Status: %d\n", rc);
  // HAL_UART_Transmit(&huart1, (const uint8_t *)error, error_len, 1000);

  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
