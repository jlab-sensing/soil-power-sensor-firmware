
#include <Arduino.h>
#include "Wire.h"
#include <WiFi.h>
#include "../lib/i2c_secondary/include/i2c_secondary.hpp"

#define I2C_DEV_ADDR 0x6B
#define SCL 1
#define SDA 0

uint32_t i = 0;

void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1);  // Send ACK
    Serial.print("Sent ACK, length: ");
    Serial.println(sizeof(ack) - 1);
}

void onReceive(int len){
  Serial.printf("onReceive[%d]: ", len);
  while(Wire.available()){
    Serial.write(Wire.read());
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("I2C Slave");
  Serial.println(WiFi.macAddress());
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR, SDA, SCL, 100000);
  //Wire.begin((uint8_t)I2C_DEV_ADDR);
  Serial.println("I2C began");


#if CONFIG_IDF_TARGET_ESP32
  char message[64];
  snprintf(message, 64, "%lu Packets.", i++);
  Wire.slaveWrite((uint8_t *)message, strlen(message));
#endif
}

void loop() {
  
}