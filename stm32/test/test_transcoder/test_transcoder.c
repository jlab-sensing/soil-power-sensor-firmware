/**
 * @file test_template.c
 * @brief Template file for tests
 * 
 * @author John Madden <jmadden173@pm.me>
 * @date 2024-01-05
*/

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>

#include <unity.h>

#include "transcoder.h"

void SystemClock_Config(void);
  

/**
 * @brief Setup code that runs at the start of every test
*/
void setUp(void) {}

/**
 * @brief Tear down code that runs at the end of every test
*/
void tearDown(void) {}

void TestEncodePower(void)
{
  uint8_t buffer[256];
  size_t buffer_len;

  buffer_len = EncodePowerMeasurement(1436079600, 4, 7, 37.13, 185.29, buffer);

  uint8_t data[] = {0xa, 0xc, 0x8, 0x4, 0x10, 0x7, 0x1a, 0x6, 0x8, 0x80, 0xe7,
                 0xe1, 0xac, 0x5, 0x12, 0x12, 0x11, 0x71, 0x3d, 0xa, 0xd7,
                 0xa3, 0x90, 0x42,0x40, 0x19, 0xe1, 0x7a, 0x14, 0xae, 0x47,
                 0x29, 0x67, 0x40};
  size_t data_len = 34;

  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
}

void TestEncodeTeros12(void)
{
  uint8_t buffer[256];
  size_t buffer_len;

  buffer_len = EncodeTeros12Measurement(1436079600, 4, 7, 2124.62, 0.43, 24.8,
                                        123, buffer);

  uint8_t data[] = {0xa, 0xc, 0x8, 0x4, 0x10, 0x7, 0x1a, 0x6, 0x8, 0x80, 0xe7,
                 0xe1, 0xac, 0x5, 0x1a, 0x11, 0x15, 0xec, 0xc9, 0x4, 0x45, 0x1d,
                 0xf6, 0x28, 0xdc, 0x3e, 0x25, 0x66, 0x66, 0xc6, 0x41, 0x28,
                 0x7b};
  size_t data_len = 33;
  
  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
}

void TestDecodeResponseSuccess(void)
{
  uint8_t data[] = {};
  size_t data_len = 0;

  Response_ResponseType resp_type = DecodeResponse(data, data_len);
  
  TEST_ASSERT_EQUAL(Response_ResponseType_SUCCESS, resp_type);
}

void TestDecodeResponseError(void)
{
  uint8_t data[] = {0x8, 0x1};
  size_t data_len = 2;

  Response_ResponseType resp_type = DecodeResponse(data, data_len);
  
  TEST_ASSERT_EQUAL(Response_ResponseType_SUCCESS, resp_type);
}

/**
  * @brief Entry point for protobuf test
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  // Unit testing
  UNITY_BEGIN();

  RUN_TEST(TestEncodePower);
  RUN_TEST(TestEncodeTeros12);
  RUN_TEST(TestDecodeResponseSuccess);
  RUN_TEST(TestDecodeResponseError);

  UNITY_END();
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
  HAL_UART_Transmit(&huart1, error, error_len, 1000);

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
