#include "../../../lib/esp32i2c/include/esp32i2c.h"
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

// State definitions
typedef enum {
    STATE_IDLE,
    STATE_TRANSMIT_DATA,
    STATE_RECEIVE_ACK,
    STATE_ERROR
} State;

// Global variables
State currentState = STATE_IDLE;
I2CData g_I2CData;
extern uint32_t counter;  // Declare as extern
volatile bool taskRunning = false;

void SystemClock_Config(void);

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

void runStateMachine(void) {
    if (taskRunning) {
        // If the task is already running, skip scheduling another one
        return;
    }
    taskRunning = true;
    switch (currentState) {
        case STATE_IDLE:
            updateData();
            currentState = STATE_TRANSMIT_DATA;
            break;

        case STATE_TRANSMIT_DATA:
            TransmitI2C();
            currentState = STATE_RECEIVE_ACK;
            break;

        case STATE_RECEIVE_ACK:
            ReceiveACK();
            currentState = STATE_IDLE;
            break;

        case STATE_ERROR:
            // Handle error (e.g., log, retry, etc.)
            HAL_UART_Transmit(&huart1, (uint8_t*)"Error occurred\n", 15, 1000);
            currentState = STATE_IDLE;
            break;
    }
    taskRunning = false;
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
    updateData();

    // Send UART message
    char info_str[100];
    sprintf(info_str, "I2C Master Controller initialized, compiled on %s %s\n", __DATE__, __TIME__);
    HAL_UART_Transmit(&huart1, (uint8_t*)info_str, strlen(info_str), 1000);

    // Register I2C transmission task
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_TransmitI2C), UTIL_SEQ_RFU, runStateMachine);

    // Create and start a timer for I2C transmission
    UTIL_TIMER_Object_t i2cTimer;
    UTIL_TIMER_Create(&i2cTimer, 4000, UTIL_TIMER_PERIODIC, onTransmitI2C, NULL);
    UTIL_TIMER_Start(&i2cTimer);

    while (1) {
        UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure LSE Drive Capability */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Configure the main internal regulator output voltage */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

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

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
