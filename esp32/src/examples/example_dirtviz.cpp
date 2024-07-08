/**
 * Copyright 2024 jLab
 * @brief Example of using dirtviz library
 * 
 * @author John Madden <jmadden173@pm.me>
 * @date 2023-11-30
 */

#include <Arduino.h>
#include <WiFi.h>

#include "../lib/dirtviz/include/dirtviz.hpp"

/** Baud rate for serial interface */
#define SERIAL_BAUD 115200

const char ssid[] = "wifi-network-here";
const char pass[] = "wifi-password-here";

Dirtviz api("ip address here", 8080);

const char data[] = "message-here";
const size_t data_len = 21;

/**
 * @brief Initialization code run on startup
 * 
 * 
*/
void setup() {
  // Start serial interface
  Serial.begin(115200);
  // Wait for serial connection
  while (!Serial) { delay(100); }

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(pass);
  Serial.print("Connecting");

  // Connect to WiFi network
  WiFi.begin(ssid, pass);

  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
}

void loop() {
  int resp_code;

  const uint8_t *resp_data;
  size_t resp_data_len;

  // Send example measurement
  resp_code = api.SendMeasurement((const uint8_t*) data, data_len);
  Serial.print("Response Code: ");
  Serial.println(resp_code);

  // Get response data
  resp_data_len = api.GetResponse(resp_data);
  Serial.println("Response:");
  Serial.write(resp_data, resp_data_len);

  // wait 1s
  delay(1000);
}
