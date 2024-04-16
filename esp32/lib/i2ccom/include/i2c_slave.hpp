#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP

#include <Arduino.h>
#include <Wire.h>

class ESP32_I2C_Slave {
public:
    ESP32_I2C_Slave(uint8_t address);
    void begin();
    String readMessage();
    static void receiveEvent(int howMany);

private:
    static String incomingMessage;
    static int expectedBytes;
    static bool isFirstByte;
    static uint8_t slaveAddress;
};

#endif
