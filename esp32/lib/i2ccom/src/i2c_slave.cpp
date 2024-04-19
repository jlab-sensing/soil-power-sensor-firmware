// i2c_slave.cpp
#include "i2c_slave.hpp"
#define SCL_PIN 1
#define SDA_PIN 0
#define I2C_DEV_ADDR 0x6B

static uint32_t packetCount = 0;

void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1); // Send ACK after processing the message
    Serial.println("Sent acknowledge");
    Serial.print("onRequest: Sent ");
    Serial.println(ack);
}

void onReceive(int len) {
    Serial.printf("onReceive[%d]: ", len);
    Wire.read(); // skip reading the size byte
    while (Wire.available()) {
        char c = Wire.read();
        Serial.print(c);
    }
    Serial.println();
}

void initI2C() {
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA_PIN, SCL_PIN, 100000); // ESP32 specific initialization
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.println("I2C configured for device address: 0x6B");
}