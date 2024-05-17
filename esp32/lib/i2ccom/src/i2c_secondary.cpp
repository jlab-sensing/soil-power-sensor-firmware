#include "i2c_secondary.hpp"

#define I2C_DEV_ADDR 0x6B
#define SCL_PIN 1
#define SDA_PIN 0

static i2c_error_t i2cLastError = I2C_OK;
static uint8_t dataBuffer[MAX_BUFFER_SIZE];
static int dataCount = 0;

void resetDataBuffer() {
    memset(dataBuffer, 0, MAX_BUFFER_SIZE);
    dataCount = 0;
}

void onReceive(int len) {
    resetDataBuffer();
    if (len > MAX_BUFFER_SIZE) {
        i2cLastError = I2C_ERROR_BUFFER_OVERFLOW;
        Serial.println("Error: Buffer overflow.");
        return;
    }
    Serial.printf("onReceive[%d]: ", len);
    while (Wire.available() && dataCount < MAX_BUFFER_SIZE) {
        uint8_t c = Wire.read();
        dataBuffer[dataCount++] = c;
    }
    i2cLastError = I2C_OK; // Reset error after successful operation
    Serial.println("Data stored");
}

void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1);
    i2cLastError = I2C_OK;
    Serial.println("Sent acknowledge");
}

void initI2C() {
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA_PIN, SCL_PIN, 100000);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.begin(115200);
    Serial.println("I2C configured for device address: 0x6B");
}

i2c_error_t getI2CError() {
    return i2cLastError;
}

int getDataCount() {
    return dataCount;
}

void clearI2CError() {
    i2cLastError = I2C_OK;
}