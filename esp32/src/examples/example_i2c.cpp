#include <Arduino.h>
#include "i2c_slave.hpp"

ESP32_I2C_Slave i2cSlave(0x40);  // Instantiate the I2C slave with address 0x28

void setup() {
    Serial.begin(115200);  // Initialize serial communication at 115200 baud
    Serial.println("Starting up I2C Slave device...");
    i2cSlave.begin();  // Initialize I2C as a slave device
    Serial.println("I2C Slave device configured and ready to receive messages.");
}

void loop() {
    // The received messages are handled in the receiveEvent interrupt, no need to check here
    // Instead, just check if there's a message ready and print it
    String msg = i2cSlave.readMessage();
    if (!msg.isEmpty()) {
        Serial.println("Received Message: " + msg);
        // After printing the message, it's already cleared by readMessage function
    }
    delay(100);  // Slight delay to prevent flooding the serial output
}