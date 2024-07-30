// Copyright 2023 UCSC

/**
 * @file     fram.c
 * @author   Stephen Taylor
 * @brief    This file contains all the function definitions for the fram library. 
 * 
 * Copyright 2023 jLab, UCSC MIT License
 */
#include "include/fram.h"

/** Timeout in seconds for the FRAM chip */
const uint32_t fram_timeout = 100;

/** Device I2C address */
const uint16_t fram_i2c_addr = 0b10100000;

/** Device address mask for read */
const uint16_t fram_read_mask = 0b1;

/** Device address mask for write */
const uint16_t fram_write_mask = 0b0;

/** Matrix representation of memory */
typedef struct {
  /** Page reference */
  uint16_t page;
  /** Segment address */
  uint16_t seg;
} FramAddress;

/**
 * @brief Convert an address to matrix memory representation
 *
 * No out of memory errors are checked.
 *
 * @param addr Memory address
 * @return Matrix representation of address
 */
FramAddress FramConvertAddrMem(uint16_t addr);

FramStatus FramWrite(uint16_t addr, const uint8_t *data, uint8_t len) {
  // Write byte array to memory
  // NOTE write is performed a single byte at a time due to address
  // configuration of the chip.
  for (uint8_t *d = data; d < data+len; d++) {
    FramAddress addr_mat = FramConvertAddrMem(addr);

    // check for out of memory
    // pages are zero indexed
    if (addr_mat.page > FRAM_PAGES-1) {
      return FRAM_OUT_OF_RANGE;
    }

    // format the i2c address
    uint16_t i2c_addr = fram_i2c_addr | (addr_mat.page << 1) | fram_write_mask;

    // Write byte to an address
    // Can optimize by using continuous writes but needs to account for the
    // change of address.
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Write(&hi2c2, i2c_addr, addr_mat.seg,
                               I2C_MEMADD_SIZE_8BIT, d, 1, fram_timeout);
    // return error if failed to write
    if (status != HAL_OK) {
      return FRAM_ERROR;
    }

    // increment address
    ++addr;
  }

  return FRAM_OK;
}

FramStatus FramRead(uint16_t addr, uint8_t len, uint8_t *data) {
  // Write byte array to memory
  // NOTE write is performed a single byte at a time due to address
  // configuration of the chip.
  for (uint8_t *d = data; d < data+len; d++) {
    FramAddress addr_mat = FramConvertAddrMem(addr);

    // check for out of memory
    if (addr_mat.page >= FRAM_PAGES) {
      return FRAM_OUT_OF_RANGE;
    }
    // format the i2c address
    uint16_t i2c_addr = fram_i2c_addr | (addr_mat.page << 1) | fram_read_mask;

    // Read byte to data address
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read(&hi2c2, i2c_addr, addr_mat.seg,
                              I2C_MEMADD_SIZE_8BIT, d, 1, fram_timeout);

    // return error if failed to write
    if (status != HAL_OK) {
      return FRAM_ERROR;
    }

    // increment address
    ++addr;
  }

  return FRAM_OK;
}

HAL_StatusTypeDef ConfigureSettings(configuration c) {
  HAL_StatusTypeDef status = HAL_OK;

  // TODO(GSOC student) implement user config write

  return status;
}

configuration ReadSettings(void) {
  configuration c;

  // TODO(GSOC student) implement user config read

  return c;
}

FramAddress FramConvertAddrMem(uint16_t addr) {
  // convert flat address space to matrix
  FramAddress addr_mat;
  addr_mat.page = addr / FRAM_SEG_SIZE;
  addr_mat.seg = addr % FRAM_SEG_SIZE;

  return addr_mat;
}
