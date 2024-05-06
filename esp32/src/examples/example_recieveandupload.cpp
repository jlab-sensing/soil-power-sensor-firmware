/**
 * @brief Example of recieving soil power sensor data from STM32 and uploading it over wifi
 * 
 * @author Varun Sreedharan
 * @date 2023-04-19
 */

#include <Arduino.h>
#include <WiFi.h>

#include "dirtviz.hpp"
#include "i2c_secondary.hpp"

/** Baud rate for serial interface */
#define SERIAL_BAUD 115200

const char ssid[] = "yourssid";
const char pass[] = "yourpassword";

Dirtviz api("yourip", 8080);

void setup() {
    Serial.begin(115200);
    // Wait for serial connection
    while (!Serial) { 
        delay(100); 
    }
    WiFi.begin(ssid, pass);
    // Wait for WiFi to connect
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Connected!");
    Serial.println(WiFi.localIP());
    initI2C(); // Initialize I2C communication

}

void loop() {
    int resp_code;
    const uint8_t *data;
    size_t data_len;

    // Fetch the data from the I2C buffer
    data = returnDataBuffer();
    data_len = getDataCount();

    // Send the fetched data
    api.SendMeasurement(data, data_len);

    delay(1000);  // Adjust the delay as needed
    resetDataBuffer();
}