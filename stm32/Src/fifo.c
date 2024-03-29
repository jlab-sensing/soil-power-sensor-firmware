/**
 * @file    fifo.c
 * @author  Stephen Taylor
 * @date    11/17/2023
 * 
 * @see fifo.h
 **/

#include "fifo.h"

static const uint16_t buffer_size = FRAM_BUFFER_END - FRAM_BUFFER_START;

static uint16_t read_addr = 0;
static uint16_t write_addr = 0;

/** Number of measurements stored in buffer */
static uint16_t buffer_len = 0;

/**
 * @brief Updates circular buffer address based on number of bytes
 * 
 * @param addr 
 * @param num_bytes 
 */
static inline void update_addr(uint16_t *addr, const uint16_t num_bytes) {
  *addr = (*addr + num_bytes) % buffer_size;
}

FramStatus fram_put(const uint8_t *data, const uint16_t num_bytes) {
  // check remaining space
  uint16_t remaining_space = buffer_size - (write_addr - read_addr);
  if (num_bytes+1 > remaining_space) {
    return FRAM_BUFFER_FULL;
  }

  FramStatus status;

  // write single byte length to buffer
  status = FRAM_Write(write_addr, &num_bytes, 1);
  if (status != FRAM_SUCCESS) {
    return status;
  }
  update_addr(&write_addr, 1);

  // Write data to FRAM circular buffer
  FramStatus status = FRAM_Write(write_addr, data, num_bytes);
  if (status != FRAM_SUCCESS) {
    return status;
  }
  update_addr(&write_addr, num_bytes);

  // increment buffer length
  ++buffer_len;

  return FRAM_SUCCESS;
}

FramStatus fram_get(uint8_t *data) {
  // Check if buffer is empty
  if (buffer_len == 0) {
    return FRAM_BUFFER_EMPTY;
  }

  FramStatus status;

  uint8_t len;
  status = FRAM_Read(read_addr, 1, &len);
  if (status != FRAM_SUCCESS) {
    return status;
  }
  update_addr(&read_addr, 1);
  

  // Read data from FRAM circular buffer
  status = FRAM_Read(read_addr, len, data);
  if (status != FRAM_SUCCESS) {
    return status;
  }
  update_addr(&read_addr, len);

  // Decrement buffer length
  --buffer_len;

  return FRAM_SUCCESS;
}

uint16_t fram_buffer_len(void) {
  return buffer_len;
}