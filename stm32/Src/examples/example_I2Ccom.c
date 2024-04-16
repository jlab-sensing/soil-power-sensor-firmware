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
    HAL_Init();  // Reset of all peripherals, Initializes the Flash interface and the Systick.
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_I2C2_Init();

    char info_str[100];
    int info_len = sprintf(info_str, "I2C Master Controller initialized, compiled on %s %s\n", __DATE__, __TIME__);
    HAL_UART_Transmit(&huart1, (const uint8_t*) info_str, info_len, 1000);

    uint8_t data[] = {0x05, 'H', 'e', 'l', 'l', 'o'}; // Example data to send
    char output[100];
    int output_len;

    while (1)
    {
        HAL_StatusTypeDef result = I2C2_Transmit(0x40, data, sizeof(data));
        if (result == HAL_OK) {
            output_len = sprintf(output, "I2C Transmission successful.\n");
        } else {
            output_len = sprintf(output, "I2C Transmission failed, HAL status: %d\n", result);
        }
        HAL_UART_Transmit(&huart1, (const uint8_t*) output, output_len, 1000);
        uint8_t ackBuffer[3]; // Buffer to receive ACK
        result = HAL_I2C_Master_Receive(&hi2c2, (0x40 << 1), ackBuffer, sizeof(ackBuffer), 1000);
        if (result == HAL_OK && strncmp((char *)ackBuffer, "ACK", 3) == 0) {
            output_len = sprintf(output, "ACK received.\n");
        } else {
            output_len = sprintf(output, "No ACK received, HAL status: %d\n", result);
        }
        HAL_UART_Transmit(&huart1, (const uint8_t*) output, output_len, 1000);
        HAL_Delay(1000);  // 1 second delay between transmissions
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
