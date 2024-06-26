/**
 * @file test_transcoder.c
 * 
 * @brief Tests encoding/decoding against binary data obtained from python
 * 
 * Encoding of a Measurement message is compared against serialized data from
 * python using the same input parameters. A byte wise comparison is used for
 * these tests.
 * 
 * Decoding a Response message takes binary data from python and compares the
 * decoded ResponseType to the expected. Both a SUCCESS and ERROR are checked to
 * cover the default case where "no" data is passed into the function.
 * 
 * The parameters used for Measurement and Response messages are located in
 * generate_bytes.py and can be used to generate C code to copy into this test
 * file.
 * 
 * @see generate_bytes.py
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


/**
 * @brief Generated from CubeMX
*/
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

  buffer_len = EncodePowerMeasurement(1436079600, 7, 4, 37.13, 185.29, buffer);
  
  uint8_t data[] = {0xa, 0xa, 0x8, 0x4, 0x10, 0x7, 0x18, 0xf0, 0xab, 0xe3, 0xac,
                    0x5, 0x12, 0x12, 0x11, 0x71, 0x3d, 0xa, 0xd7, 0xa3, 0x90,
                    0x42, 0x40, 0x19,0xe1, 0x7a, 0x14, 0xae, 0x47, 0x29, 0x67,
                    0x40};
  size_t data_len = 32;

  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
}

void TestEncodeTeros12(void)
{
  uint8_t buffer[256];
  size_t buffer_len;

  buffer_len = EncodeTeros12Measurement(1436079600, 7, 4, 2124.62, 0.43, 24.8,
                                        123, buffer);

  uint8_t data[] = {0xa, 0xa, 0x8, 0x4, 0x10, 0x7, 0x18, 0xf0, 0xab, 0xe3,
                    0xac, 0x5, 0x1a, 0x1d, 0x11, 0xa, 0xd7, 0xa3, 0x70, 0x3d,
                    0x99, 0xa0, 0x40, 0x19, 0x85, 0xeb, 0x51, 0xb8, 0x1e,
                    0x85, 0xdb, 0x3f, 0x21, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc,
                    0xcc, 0x38, 0x40, 0x28, 0x7b};
  size_t data_len = 43;

  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
}

void TestEncodePhytos31(void)
{
  uint8_t buffer[256];
  size_t buffer_len;

  buffer_len = EncodePhytos31Measurement(1436079600, 7, 4, 1425.12, 1962.2,
                                         buffer);

  uint8_t data[] = {0xa, 0xa, 0x8, 0x4, 0x10, 0x7, 0x18, 0xf0, 0xab, 0xe3, 0xac,
                    0x5, 0x22, 0x12, 0x9, 0x14, 0xae, 0x47, 0xe1, 0x7a, 0x44,
                    0x96, 0x40, 0x11, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0xa8, 0x9e,
                    0x40};
  size_t data_len = 32;

  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
}

void TestEncodeBME280(void)
{
  uint8_t buffer[256];
  size_t buffer_len;

  buffer_len = EncodeBME280Measurement(1436079600, 7, 4, 98473, 2275, 43600,
                                       buffer);

  uint8_t data[] = {0xa, 0xa, 0x8, 0x4, 0x10, 0x7, 0x18, 0xf0, 0xab, 0xe3, 0xac,
                    0x5, 0x2a, 0xb, 0x8, 0xa9, 0x81, 0x6, 0x10, 0xe3, 0x11, 0x18,
                    0xd0, 0xd4, 0x2};
  size_t data_len = 25;

  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer, buffer_len);
  TEST_ASSERT_EQUAL_INT(data_len, buffer_len);
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
  
  TEST_ASSERT_EQUAL(Response_ResponseType_ERROR, resp_type);
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

  // wait for UART
  for (int i = 0; i < 1000000; i++) {
      __NOP();
  }

  // Unit testing
  UNITY_BEGIN();

  RUN_TEST(TestEncodePower);
  RUN_TEST(TestEncodeTeros12);
  RUN_TEST(TestEncodePhytos31);
  RUN_TEST(TestEncodeBME280);
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
