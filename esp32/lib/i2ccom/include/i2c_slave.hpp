// i2c_slave.hpp
#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP

#include <Arduino.h>
#include <Wire.h>


void onRequest();
void onReceive(int len);
void initI2C();

#endif // I2C_COMM_HPP
