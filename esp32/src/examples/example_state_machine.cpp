#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "../include/dirtviz.hpp"

#define I2C_DEV_ADDR 0x6B
#define SCL 1
#define SDA 0

const char ssid[] = "ssid";
const char pass[] = "password";

enum State {
    STATE_IDLE,
    STATE_CONNECT_WIFI,
    STATE_PROCESSING,
    STATE_STORE_LOCALLY,
    STATE_ERROR
};

State currentState = STATE_IDLE;
Dirtviz dirtviz("192.168.49.155", 8080);
bool wifiConnected = false;
File file;
uint8_t receivedData[256]; // Buffer to store received data
size_t receivedDataLen = 0; // Length of received data

void onRequest() {
    const char ack[] = "ACK";
    Wire.write((const uint8_t*)ack, sizeof(ack) - 1);  // Send ACK
    Serial.print("Sent ACK, length: ");
    Serial.println(sizeof(ack) - 1);
}

void onReceive(int len) {
    Serial.printf("onReceive[%d]: ", len);
    receivedDataLen = len;
    for (int i = 0; i < len; i++) {
        receivedData[i] = Wire.read();
        Serial.write(receivedData[i]);
    }
    Serial.println();

    if (currentState == STATE_IDLE) {
        currentState = STATE_CONNECT_WIFI;
    } else if (currentState == STATE_STORE_LOCALLY || currentState == STATE_ERROR) {
        // Ensure we don't write the same data twice
        file = SPIFFS.open("/data.txt", FILE_APPEND);
        if (file) {
            file.write(receivedData, receivedDataLen);
            file.close();
            Serial.println("Data stored locally");
        } else {
            Serial.println("Failed to store data locally");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("I2C Slave");

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        currentState = STATE_ERROR;
        return;
    }

    // Delete data.txt file if it exists
    if (SPIFFS.exists("/data.txt")) {
        SPIFFS.remove("/data.txt");
        Serial.println("data.txt deleted on startup");
    }

    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA, SCL, 100000);
    Serial.println("I2C began");
}

void printFileContents() {
    file = SPIFFS.open("/data.txt", FILE_READ);
    if (file) {
        Serial.println("Contents of /data.txt:");
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
        Serial.println(); // Newline for readability
    } else {
        Serial.println("Failed to open /data.txt for reading");
    }
}

void loop() {
    printFileContents(); // Print the contents of the data.txt file

    unsigned long startTime; // Declare variables outside of switch
    int status;

    switch (currentState) {
        case STATE_IDLE:
            Serial.println("State: IDLE");
            delay(100);
            break;

        case STATE_CONNECT_WIFI:
            Serial.println("State: CONNECT_WIFI");
            WiFi.begin(ssid, pass);
            startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
                delay(500);
                Serial.print(".");
            }
            if (WiFi.status() == WL_CONNECTED) {
                wifiConnected = true;
                const char wifi_status[] = "WIFI_OK";
                Wire.write((const uint8_t*)wifi_status, sizeof(wifi_status) - 1);  // Send successful WiFi ack
                currentState = STATE_PROCESSING;
            } else {
                wifiConnected = false;
                const char wifi_status[] = "WIFI_FAIL";
                Wire.write((const uint8_t*)wifi_status, sizeof(wifi_status) - 1);  // Send fail WiFi ack
                currentState = STATE_STORE_LOCALLY;
            }
            break;

        case STATE_PROCESSING:
            Serial.println("State: PROCESSING");
            // Send received data
            status = dirtviz.SendMeasurement(receivedData, receivedDataLen);
            if (status == 200) {
                Serial.println("Data sent successfully");
                // Read and send stored data
                file = SPIFFS.open("/data.txt");
                if (file) {
                    while (file.available()) {
                        size_t len = file.read(receivedData, sizeof(receivedData));
                        dirtviz.SendMeasurement(receivedData, len);
                    }
                    file.close();
                    SPIFFS.remove("/data.txt");
                    Serial.println("Stored data sent and file cleared");
                }
                currentState = STATE_IDLE;
            } else {
                Serial.println("Failed to send data");
                currentState = STATE_STORE_LOCALLY;
            }
            break;

        case STATE_STORE_LOCALLY:
            Serial.println("State: STORE_LOCALLY");
            // Handle local data storage
            file = SPIFFS.open("/data.txt", FILE_APPEND);
            if (file) {
                file.write(receivedData, receivedDataLen);
                file.close();
                Serial.println("Data stored locally");
                currentState = STATE_IDLE;
            } else {
                Serial.println("Failed to store data locally");
                currentState = STATE_ERROR;
            }
            break;

        case STATE_ERROR:
            Serial.println("State: ERROR");
            delay(100);
            break;
    }
    delay(100);
}