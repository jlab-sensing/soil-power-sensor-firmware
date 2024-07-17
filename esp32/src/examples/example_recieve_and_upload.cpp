/**
 * @brief Example of recieving soil power sensor data from STM32 and uploading it over wifi
 * 
 * @author Varun Sreedharan
 * @date 2024-04-19
 */

#include <Arduino.h>
#include "Wire.h"
#include "WiFi.h"

#include "../lib/dirtviz/include/dirtviz.hpp"

#define I2C_DEV_ADDR 0x6B
#define SCL 1
#define SDA 0

const char ssid[] = "ssid";
const char pass[] = "password";
const char serverIp[] = "ip address";  // Your computer's IP address
const uint16_t serverPort = 8080;

Dirtviz api(serverIp, serverPort);

uint8_t i2cReceiveBuffer[256];  // Buffer to store I2C data
size_t i2cReceiveLength = 0;

void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1);
    Serial.println("Sent ACK");
}

void onReceive(int numBytes) {
    if (numBytes > sizeof(i2cReceiveBuffer)) {
        Serial.println("Error: Buffer overflow");
        return;
    }

    i2cReceiveLength = 0;
    while (Wire.available()) {
        if (i2cReceiveLength < numBytes) {
            i2cReceiveBuffer[i2cReceiveLength++] = Wire.read();
        }
    }

    Serial.print("Received: ");
    for (size_t i = 0; i < i2cReceiveLength; i++) {
        Serial.print(i2cReceiveBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Send the received data over WiFi
    if (WiFi.status() == WL_CONNECTED) {
        int respCode = api.SendMeasurement(i2cReceiveBuffer, i2cReceiveLength);
        Serial.print("Response Code: ");
        Serial.println(respCode);
    } else {
        Serial.println("WiFi not connected, data not sent.");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA, SCL, 100000);

}

void loop() {
    // Handling of data reception and transmission is done in the onReceive function
    delay(1000);  // Loop delay for stability
}