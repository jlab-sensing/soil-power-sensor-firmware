#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "../include/dirtviz.hpp"

#define I2C_DEV_ADDR 0x6B
#define SCL 1
#define SDA 0

const char ssid[] = "ssid";
const char pass[] = "pass";

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
uint8_t receivedData[256]; // Buffer to store received data
size_t receivedDataLen = 0; // Length of received data
bool sendAck = false; // Flag to indicate whether to send an acknowledgment
bool sendStoreLocallyAck = false; // Flag to indicate whether to send a store locally acknowledgment

void onRequest() {
    if (sendAck) {
        const char ack[] = "ACK";
        Wire.write((const uint8_t*)ack, sizeof(ack) - 1);  // Send ACK
        Serial.print("Sent ACK, length: ");
        Serial.println(sizeof(ack) - 1);
        sendAck = false; // Reset the flag after sending ACK
    } else if (sendStoreLocallyAck) {
        const char store_locally_ack[] = "STORE_LOCALLY";
        Wire.write((const uint8_t*)store_locally_ack, sizeof(store_locally_ack) - 1);  // Send store locally ACK
        Serial.println("Sent STORE_LOCALLY ack to STM32");
        sendStoreLocallyAck = false; // Reset the flag after sending store locally ACK
    }
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
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("I2C Slave");

    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin((uint8_t)I2C_DEV_ADDR, SDA, SCL, 100000);
    Serial.println("I2C began");
}

void loop() {
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
            Serial.println();
            if (WiFi.status() == WL_CONNECTED) {
                wifiConnected = true;
                currentState = STATE_PROCESSING;
            } else {
                wifiConnected = false;
                currentState = STATE_STORE_LOCALLY;
            }
            break;

        case STATE_PROCESSING:
            Serial.println("State: PROCESSING");
            // Send received data
            status = dirtviz.SendMeasurement(receivedData, receivedDataLen);
            if (status == 200) {
                Serial.println("Data sent successfully");
                sendAck = true; // Set the flag to send ACK in onRequest
                currentState = STATE_IDLE;
            } else {
                Serial.println("Failed to send data");
                sendStoreLocallyAck = true; // Set the flag to send STORE_LOCALLY ack in onRequest
                currentState = STATE_STORE_LOCALLY;
            }
            break;

        case STATE_STORE_LOCALLY:
            Serial.println("State: STORE_LOCALLY");
            // Set the flag to send STORE_LOCALLY ack in onRequest
            sendStoreLocallyAck = true;
            currentState = STATE_IDLE;
            break;

        case STATE_ERROR:
            Serial.println("State: ERROR");
            delay(100);
            break;
    }
    delay(100);
}
