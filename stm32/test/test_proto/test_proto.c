/**
 * @file test_proto.c
 * @brief Tests ability to encode and decode protobuf messages
 * 
 * Encoding/decoding of each message type is tested to ensure functionality of
 * nanopb and correctly matched tests with generate protobuf source files.
 * 
 * @author John Madden <jmadden173@pm.me>
 * @date 2023-11-27
*/

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>

#include <unity.h>

#include "pb_encode.h"
#include "pb_decode.h"

#include "soil_power_sensor.pb.h"



#ifndef UNIX_EPOCHS
/** Unix epochs encoded in timestamp. Default corresponds to Mon Nov 27 2023
 * 03:25:19 GMT+0000 */
#define UNIX_EPOCHS 1701055519
#endif /* UNIX_EPOCHS */

#ifndef FLOAT_DELTA
/** Delta between comparison between floating point numbers */
#define FLOAT_DELTA 1e-4
#endif /* FLOAT_DELTA */


void SystemClock_Config(void);
  

/** Buffer for encoded data */
uint8_t buffer[256];
/** message length */
size_t msg_len;

/** Nanopb output buffer */
pb_ostream_t ostream;
/** Nanopb input buffer */
pb_istream_t istream;

/** Nanopb status code */
bool status;

/**
 * @brief Default MeasurementMetadata
 * 
 * Uses as submessage when testing parent messages. Defaults to zeros with
 * ts_default
 * 
 * @see ts_default
*/
MeasurementMetadata meta_default;

/**
 * @brief Setup code that runs at the start of every test
 * 
 * Creates required variables that are shared between all tests. The output
 * buffer is initialized based on length of the buffer. Input buffer is defined,
 * but not initialized since it depends on the message length. Default values
 * are defined for Timestamp and MeasurementMetadata.
*/
void setUp(void)
{
  // create output buffer
  ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));


  // Set default measurement metadata
  meta_default.ts = UNIX_EPOCHS;
  meta_default.cell_id = 0;
  meta_default.logger_id = 0;
}

/**
 * @brief Tear down code that runs at the end of every test
*/
void tearDown(void) {}

void test_meta(void)
{
  MeasurementMetadata meta_encode = MeasurementMetadata_init_zero;

  meta_encode.ts = UNIX_EPOCHS;
  meta_encode.cell_id = 0;
  meta_encode.logger_id = 1;

  status = pb_encode(&ostream, MeasurementMetadata_fields, &meta_encode);
  msg_len = ostream.bytes_written;

  TEST_ASSERT_TRUE(status);
  TEST_ASSERT_GREATER_THAN(0, msg_len);


  MeasurementMetadata meta_decode;

  istream = pb_istream_from_buffer(buffer, msg_len);
  status = pb_decode(&istream, MeasurementMetadata_fields, &meta_decode);

  TEST_ASSERT_TRUE(status);

  TEST_ASSERT_EQUAL(0, meta_decode.cell_id);
  TEST_ASSERT_EQUAL(1, meta_decode.logger_id);

  TEST_ASSERT_EQUAL(UNIX_EPOCHS, meta_decode.ts);

}

void test_power(void)
{
  Measurement power_encode = Measurement_init_zero;
  power_encode.which_measurement = Measurement_power_tag;
  power_encode.has_meta = true;
  power_encode.meta = meta_default;
  power_encode.measurement.power.voltage = 1.2;
  power_encode.measurement.power.current = 120.1;

  status = pb_encode(&ostream, Measurement_fields, &power_encode);
  msg_len = ostream.bytes_written;

  TEST_ASSERT_TRUE(status);
  TEST_ASSERT_GREATER_THAN(0, msg_len);


  Measurement power_decode;

  istream = pb_istream_from_buffer(buffer, msg_len);
  status = pb_decode(&istream, Measurement_fields, &power_decode);

  TEST_ASSERT_TRUE(status);

  // Meta
  TEST_ASSERT_TRUE(power_decode.has_meta);
  TEST_ASSERT_EQUAL(0, power_decode.meta.cell_id);
  TEST_ASSERT_EQUAL(0, power_decode.meta.logger_id);
  // Timestamp
  TEST_ASSERT_EQUAL(UNIX_EPOCHS, power_decode.meta.ts);

  TEST_ASSERT_EQUAL(Measurement_power_tag, power_decode.which_measurement);
  TEST_ASSERT_DOUBLE_WITHIN(FLOAT_DELTA, 1.2,
                            power_decode.measurement.power.voltage);
  TEST_ASSERT_DOUBLE_WITHIN(FLOAT_DELTA, 120.1,
                            power_decode.measurement.power.current);
}

void test_teros12(void)
{
  Measurement teros12_encode = Measurement_init_zero;
  teros12_encode.which_measurement = Measurement_teros12_tag;
  teros12_encode.has_meta = true;
  teros12_encode.meta = meta_default;
  teros12_encode.measurement.teros12.ec = 200;
  teros12_encode.measurement.teros12.temp = 24.5;
  teros12_encode.measurement.teros12.vwc_adj = 32.4;
  teros12_encode.measurement.teros12.vwc_raw = 3200.2;

  status = pb_encode(&ostream, Measurement_fields, &teros12_encode);
  msg_len = ostream.bytes_written;

  TEST_ASSERT_TRUE(status);
  TEST_ASSERT_GREATER_THAN(0, msg_len);


  Measurement teros12_decode;

  istream = pb_istream_from_buffer(buffer, msg_len);
  status = pb_decode(&istream, Measurement_fields, &teros12_decode);

  TEST_ASSERT_TRUE(status);

  // Meta
  TEST_ASSERT_TRUE(teros12_decode.has_meta);
  TEST_ASSERT_EQUAL(0, teros12_decode.meta.cell_id);
  TEST_ASSERT_EQUAL(0, teros12_decode.meta.logger_id);
  // Timestamp
  TEST_ASSERT_EQUAL(UNIX_EPOCHS, teros12_decode.meta.ts);

  TEST_ASSERT_EQUAL(Measurement_teros12_tag, teros12_decode.which_measurement);
  TEST_ASSERT_EQUAL(200, teros12_decode.measurement.teros12.ec);
  TEST_ASSERT_DOUBLE_WITHIN(FLOAT_DELTA, 24.5,
                            teros12_decode.measurement.teros12.temp);
  TEST_ASSERT_DOUBLE_WITHIN(FLOAT_DELTA, 32.4,
                            teros12_decode.measurement.teros12.vwc_adj);
  TEST_ASSERT_DOUBLE_WITHIN(FLOAT_DELTA, 3200.2,
                            teros12_decode.measurement.teros12.vwc_raw);
}

void test_bme280(void) {
  Measurement bme280_encode = Measurement_init_zero;
  bme280_encode.which_measurement = Measurement_bme280_tag;
  bme280_encode.has_meta = true;
  bme280_encode.meta = meta_default;
  bme280_encode.measurement.bme280.temperature = 27;
  bme280_encode.measurement.bme280.pressure = 300;
  bme280_encode.measurement.bme280.humidity = 10;

  status = pb_encode(&ostream, Measurement_fields, &bme280_encode);
  msg_len = ostream.bytes_written;

  TEST_ASSERT_TRUE(status);
  TEST_ASSERT_GREATER_THAN(0, msg_len);

  Measurement bme280_decode;

  istream = pb_istream_from_buffer(buffer, msg_len);
  status = pb_decode(&istream, Measurement_fields, &bme280_decode);

  TEST_ASSERT_TRUE(status);

  // Meta
  TEST_ASSERT_TRUE(bme280_decode.has_meta);
  TEST_ASSERT_EQUAL(0, bme280_decode.meta.cell_id);
  TEST_ASSERT_EQUAL(0, bme280_decode.meta.logger_id);
  // Timestamp
  TEST_ASSERT_EQUAL(UNIX_EPOCHS, bme280_decode.meta.ts);

  TEST_ASSERT_EQUAL(Measurement_bme280_tag, bme280_decode.which_measurement);
  TEST_ASSERT_EQUAL(27, bme280_decode.measurement.bme280.temperature);
  TEST_ASSERT_EQUAL(300, bme280_decode.measurement.bme280.pressure);
  TEST_ASSERT_EQUAL(10, bme280_decode.measurement.bme280.humidity);
}

void test_response(void)
{
  Response response_encode = Response_init_zero;
  response_encode.resp = Response_ResponseType_SUCCESS;

  status = pb_encode(&ostream, Response_fields, &response_encode);
  msg_len = ostream.bytes_written;
  
  TEST_ASSERT_TRUE(status);


  Response response_decode;

  istream = pb_istream_from_buffer(buffer, msg_len);
  status = pb_decode(&istream, Response_fields, &response_decode);

  TEST_ASSERT_TRUE(status);

  TEST_ASSERT_EQUAL(Response_ResponseType_SUCCESS, response_decode.resp);
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

  // Tests for MeasurementMetadata
  RUN_TEST(test_meta);
  // Tests for PowerMeasurement
  RUN_TEST(test_power);
  // Tests for Teros12
  RUN_TEST(test_teros12);
  // Tests for Response
  RUN_TEST(test_response);

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
