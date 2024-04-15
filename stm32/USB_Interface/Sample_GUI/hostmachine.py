# Host Machine (Python) - hostmachine.py

import time

# Define constants
START_MARKER = b'\x02'
END_MARKER = b'\x03'
MESSAGE_CONFIG = b'C'
MESSAGE_REQUEST = b'R'

# Function to simulate sending a message
def send_message(message_type, data):
    message = START_MARKER + message_type + data + END_MARKER
    print("Simulating sending message:", message)

# Function to simulate receiving a message
def receive_message():
    # Simulated measurement data
    measurement_data = b'\x01\x02\x03\x04'
    return measurement_data

# Function to simulate communication with STM32
def run_communication():
    # Simulate sending configuration data to STM32
    config_data = b'\x01\x02\x03\x04'
    send_message(MESSAGE_CONFIG, config_data)
    print("Sent configuration data:", config_data)

    # Simulate requesting measurement data from STM32
    send_message(MESSAGE_REQUEST, b'')
    print("Requested measurement data")

    # Simulate receiving and printing measurement data from STM32
    measurement_data = receive_message()
    print("Received measurement data:", measurement_data)

# Run the communication simulation when this script is executed
if __name__ == "__main__":
    run_communication()
