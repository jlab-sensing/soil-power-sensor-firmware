// i2c_slave.cpp
#include "i2c_slave.hpp"
#define SCL_PIN 1
#define SDA_PIN 0
#define I2C_DEV_ADDR 0x6B
#define MAX_BUFFER_SIZE 242

static uint32_t packetCount = 0;
static uint8_t dataBuffer[MAX_BUFFER_SIZE];
static int dataCount = 0;

void resetDataBuffer() {
    memset(dataBuffer, 0, MAX_BUFFER_SIZE);
    dataCount = 0;
}

uint8_t* returnDataBuffer() {
    return dataBuffer;
}

int getDataCount() {
    return dataCount;
}


void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1); // Send ACK after processing the message
    Serial.println("Sent acknowledge");
    Serial.print("onRequest: Sent ");
    Serial.println(ack);
}

void onReceive(int len) {
    resetDataBuffer();
    Serial.printf("onReceive[%d]: ", len);
    Wire.read(); // skip reading the size byte
    while (Wire.available() && dataCount < MAX_BUFFER_SIZE) {
        uint8_t c = Wire.read();
        dataBuffer[dataCount] = c;
        dataCount++;
    }
    Serial.println("Data stored");
}

void initI2C() {
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA_PIN, SCL_PIN, 100000); // ESP32 specific initialization
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.println("I2C configured for device address: 0x6B");
}