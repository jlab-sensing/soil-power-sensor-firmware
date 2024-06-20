/**
 * @file test_adc.c
 * @brief unit test for adc.c
 * 
 * @author Ahmed Falah <ahmed.falah600@gmail.com>
 * @date 2024-03-18
*/

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>

#include <unity.h>

#include "ads.h"

void SystemClock_Config(void);
  
/**
 * @brief Setup code that runs at the start of every test
*/
void setUp(void) {}

/**
 * @brief Tear down code that runs at the end of every test
*/
void tearDown(void) {}

// Test case for ADC_init
void test_ADC_init_success(void) {
  HAL_StatusTypeDef ret = ADC_init();
  // Assert successful initialization
  TEST_ASSERT_EQUAL(HAL_OK, ret);
  /* We can add conditions based on the return status of the ADC_init()
     return cases : HAL_ERROR , HAL_BUSY or HAL_OK 
     we can add message with each Assertion 
  */
}

// Test case for ADC_readVoltage (focusing on function call and return value only)
void test_ADC_readVoltage_returns_value(void) {
  int voltage = ADC_readVoltage();
  /*Assert that a value is returned (doesn't matter what value)
    just check there is no problem in the I2C Transmit or receive functions*/ 
  TEST_ASSERT_NOT_EQUAL(HAL_ERROR, voltage);
  TEST_ASSERT_NOT_EQUAL(HAL_BUSY, voltage);
  /* We can add conditions based on the return status of the ADC_readVoltage()
     return cases : HAL_ERROR , HAL_BUSY or calibrated value 
     we can add message with each Assertion 
  */
}

// Test case for ADC_readVoltage conversion (for future testing)
void test_ADC_readVoltage_conversion(void) {
  TEST_IGNORE();  // Marked as ignored for now
}

// Test ADS12 to see if it is responsive.
void test_probeADS12_device_ready(void) {
  HAL_StatusTypeDef ret = probeADS12();
  // Assert a succsuful_probe
  TEST_ASSERT_EQUAL(HAL_OK, ret);
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

  // Tests for timestamp
  RUN_TEST(test_ADC_init_success);
  RUN_TEST(test_ADC_readVoltage_returns_value);
  RUN_TEST(test_ADC_readVoltage_conversion);  // Marked as ignored for now
  RUN_TEST(test_probeADS12_device_ready);

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
