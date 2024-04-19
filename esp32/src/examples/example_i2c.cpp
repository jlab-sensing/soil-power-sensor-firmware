#include <Arduino.h>
#include "i2c_slave.hpp"

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port to connect.
    Serial.println("Initializing I2C Slave Device...");

    initI2C(); // Initialize I2C communication

    Serial.println("I2C Slave Ready");
}

void loop() {
    // Nothing to do in loop
    delay(1000); // Delay to keep loop from running too fast
}