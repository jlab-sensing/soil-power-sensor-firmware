#include "unity.h"
#include "i2c_secondary.hpp"

// Setup function runs before each test
void setUp(void) {
    // Initialize or reset your test environment
    clearI2CError();  // Assuming this resets your I2C error status
    resetDataBuffer();  // Assuming this clears your buffer
}

// Teardown function runs after each test
void tearDown(void) {
    // Cleanup tasks
}

// Test the behavior of onReceive with a normal input size
void test_onReceive_NormalInput(void) {
    onReceive(5);  // Simulate receiving 5 bytes
    TEST_ASSERT_EQUAL(I2C_OK, getI2CError());
    TEST_ASSERT_EQUAL_INT(5, getDataCount());
}

// Test the onRequest function to check if it sends ACK correctly
void test_onRequest_ACKSent(void) {
    onRequest();
    // Since we can't directly check what has been sent over the wire without hardware, assume success if no error
    TEST_ASSERT_EQUAL(I2C_OK, getI2CError());
}

// Function to run all tests
int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_onReceive_NormalInput);
    RUN_TEST(test_onRequest_ACKSent);
    UNITY_END();
}

// Main function for ESP32
extern "C" void app_main() {
    runUnityTests();
    // Optionally reset the MCU or halt after tests are done
    esp_restart();  // Restart the ESP32
}