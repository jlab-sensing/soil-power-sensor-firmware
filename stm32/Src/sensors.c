/**
 * @file sensors.c
 * @author John Madden (jmadden173@pm.me)
 * @brief See sensors.h
 * @date 2024-04-01
 * 
 */

#include "sensors.h"

/** Array for holding function callbacks */
static SensorsPrototypeMeasure callback_arr[MAX_SENSORS];

/** Length of @ref callback_arr */
static unsigned int callback_arr_len = 0;

static const uint8_t buffer_size = LORAWAN_APP_DATA_BUFFER_MAX_SIZE;

/** Periodic timer for querying sensors */
static UTIL_TIMER_Object_t MeasureTimer;

static const uint32_t measure_period = MEASUREMENT_PERIOD;

/**
 * @brief Measures sensors and adds to tx buffer
 * 
 * Loops through all added callbacks and calls the functions. The resulting
 * serialized data is added to the buffer.
 */
void SensorsMeasure(void);

/**
 * @brief Runs the SensorsMeasure task
 * 
 * The task priority is set to 0 to trigger the measurement of sensors.
 */
void SensorsRun(void);

void SensorsInit(void) {
  // registers the task
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Measurement), UTIL_SEQ_RFU,
                   SensorsMeasure);

  // create the run timer
  UTIL_TIMER_Create(&MeasureTimer, measure_period, UTIL_TIMER_PERIODIC,
                    SensorsRun, NULL);
}

void SensorsStart(void) {
  // start the timer
  UTIL_TIMER_Start(&MeasureTimer);
}

void SensorsStop(void) {
  // stop the timer
  UTIL_TIMER_Stop(&MeasureTimer);
}

int SensorsAdd(SensorsPrototypeMeasure cb)
{
  // check for out of range error 
  if (callback_arr_len >= MAX_SENSORS) {
    APP_LOG(TS_OFF, VLEVEL_M, "Error: Too many sensors added!\r\n");
    return -1;
  }

  // store callback in array
  callback_arr[callback_arr_len] = cb;

  // return index and increment
  return callback_arr_len++;
}

void SensorsMeasure(void) {
  // buffer to store measurements
  uint8_t buffer[buffer_size];
  size_t buffer_len;

  // loop over callbacks
  for (int i = 0; i < callback_arr_len; i++) {
    // call measurement function
    buffer_len = callback_arr[i](buffer);
    APP_LOG(TS_ON, VLEVEL_M, "Callback index: %d\r\n", i);
    APP_LOG(TS_ON, VLEVEL_M, "Buffer length: %u\r\n", buffer_len);
    APP_LOG(TS_ON, VLEVEL_M, "Buffer: ");
    for (int j = 0; j < buffer_len; j++) {
      APP_LOG(TS_OFF, VLEVEL_M, "%x", buffer[j]);
    }
    APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
    
    // add to tx buffer
    FramStatus status = FramPut(buffer, buffer_len);
    if (status == FRAM_BUFFER_FULL) {
      APP_LOG(TS_OFF, VLEVEL_M, "Error: TX Buffer full!\r\n");
    }
    else if (status != FRAM_OK) {
      APP_LOG(TS_OFF, VLEVEL_M, "Error: General FRAM buffer!\r\n");
    } 
  }
}

void SensorsRun(void) {
  // trigger task to run
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Measurement), CFG_SEQ_Prio_0);
}
