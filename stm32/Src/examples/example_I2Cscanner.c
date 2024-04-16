#include "I2Ccom.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include <stdio.h>

void SystemClock_Config(void);

int main(void)
{
    HAL_Init(); // Initialize the HAL library
    SystemClock_Config(); // Configure the system clock
    MX_GPIO_Init(); // Initialize GPIOs
    MX_USART1_UART_Init(); // Initialize USART1
    MX_I2C2_Init(); // Initialize I2C2

    uint8_t address;
    char info[64];
    int numDevices = 0;

    HAL_UART_Transmit(&huart1, (uint8_t *)"Starting I2C scan...\r\n", 23, HAL_MAX_DELAY);

    for(address = 1; address < 128; address++)
    {
        if(HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 3, 100) == HAL_OK) // Shift address and check if device is ready
        {
            sprintf(info, "Device found at 0x%X\r\n", address);
            HAL_UART_Transmit(&huart1, (uint8_t *)info, strlen(info), HAL_MAX_DELAY);
            numDevices++;
        }
    }

    if(numDevices == 0)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)"No devices found.\r\n", 19, HAL_MAX_DELAY);
    }
    else
    {
        sprintf(info, "%d devices found.\r\n", numDevices);
        HAL_UART_Transmit(&huart1, (uint8_t *)info, strlen(info), HAL_MAX_DELAY);
    }

    while (1)
    {
        // Loop indefinitely
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);


  /* USER CODE BEGIN Error_Handler_Debug */
  char error[30];
  int error_len = sprintf(error, "Error!  HAL Status: %d\n", rc);
  HAL_UART_Transmit(&huart1, (const uint8_t *) error, error_len, 1000);

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
