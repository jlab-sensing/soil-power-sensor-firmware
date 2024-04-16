#include "i2c_slave.hpp"

String ESP32_I2C_Slave::incomingMessage = "";
int ESP32_I2C_Slave::expectedBytes = 0;
bool ESP32_I2C_Slave::isFirstByte = true;
uint8_t ESP32_I2C_Slave::slaveAddress = 0; // Initialize the static slaveAddress

ESP32_I2C_Slave::ESP32_I2C_Slave(uint8_t address) {
    slaveAddress = address; // Set the static slave address in the constructor
}

void ESP32_I2C_Slave::begin() {
    Wire.begin(slaveAddress);
    Wire.onReceive(receiveEvent);
}

void ESP32_I2C_Slave::receiveEvent(int howMany) {
    while (Wire.available()) {
        char c = Wire.read();
        if (isFirstByte) {
            expectedBytes = c; // Read the first byte to know the message length
            isFirstByte = false;
            incomingMessage = ""; // Clear any previous message
        } else {
            incomingMessage += c;
        }
    }

    if (incomingMessage.length() == expectedBytes) {
        Serial.println(incomingMessage);  // Output the complete message
        Wire.beginTransmission(slaveAddress);
        Wire.write("ACK");
        Wire.endTransmission();
        isFirstByte = true;  // Reset for the next message
    }
}

String ESP32_I2C_Slave::readMessage() {
    String msg = incomingMessage;
    incomingMessage = "";  // Clear the static variable after reading
    return msg;
}
