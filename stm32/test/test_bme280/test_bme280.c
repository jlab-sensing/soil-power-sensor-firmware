/**
 * @file test_template.c
 * @brief Tests for the BME280 environmental sensor
 * 
 * @author John Madden <jmadden173@pm.me>
 * @date 2024-01-05
 * 
 * Before running the tests, a BME280 sensor should be connected to the device.
 * The tests look at initialization, reading each channel and formatting of
 * measurement data. Values are checked such that they appears within the
 * operating range of the sensor.
*/

#include <unity.h>

#include <stdio.h>

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "sys_app.h"

#include "bme280_sensor.h"


void SystemClock_Config(void);
  

/**
 * @brief Setup code that runs at the start of every test
*/
void setUp(void) {}

/**
 * @brief Tear down code that runs at the end of every test
*/
void tearDown(void) {}


void test_init(void)
{
  BME280Status status = BME280Init();
  TEST_ASSERT_EQUAL(BME280_STATUS_OK, status);
}

void test_measure_temperature(void)
{
  BME280Data data;
  BME280Status status = BME280MeasureAll(&data);

  TEST_ASSERT_EQUAL(BME280_STATUS_OK, status);

  TEST_ASSERT_GREATER_THAN(0, data.temperature);
  TEST_ASSERT_LESS_THAN(6500, data.temperature);
}

void test_measure_pressure(void)
{
  BME280Data data;
  BME280Status status = BME280MeasureAll(&data);

  TEST_ASSERT_EQUAL(BME280_STATUS_OK, status);

  TEST_ASSERT_GREATER_THAN(3000, data.pressure);
  TEST_ASSERT_LESS_THAN(11000, data.temperature);
}

void test_measure_humidity(void)
{
  BME280Data data;
  BME280Status status = BME280MeasureAll(&data);

  TEST_ASSERT_EQUAL(BME280_STATUS_OK, status);

  TEST_ASSERT_GREATER_THAN(0, data.temperature);
  TEST_ASSERT_LESS_THAN(100000, data.temperature);
}

void test_measure(void)
{
  uint8_t buffer[256];
  size_t buffer_len = 0;

  buffer_len = BME280Measure(buffer);

  TEST_ASSERT_GREATER_THAN(0, buffer_len);
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

  SystemApp_Init();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();

  // wait for UART
  for (int i = 0; i < 1000000; i++) {
      __NOP();
  }

  // Unit testing
  UNITY_BEGIN();

  // Tests for timestamp
  RUN_TEST(test_init);
  RUN_TEST(test_measure_temperature);
  RUN_TEST(test_measure_pressure);
  RUN_TEST(test_measure_humidity);
  RUN_TEST(test_measure);

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
