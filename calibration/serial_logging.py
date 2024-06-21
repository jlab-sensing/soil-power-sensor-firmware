#!/usr/bin/env python
import pdb

import time
import argparse
import socket
import serial
import numpy as np
import pandas as pd
import pdb
import os
from tqdm import tqdm
from typing import Tuple
from soil_power_sensor_protobuf import decode_measurement

class SerialController:
    """Generic serial controller that will open and close serial connections"""

    # Serial port
    ser = None

    def __init__(self, port, baudrate=115200, xonxoff=False):
        """Constructor

        Initialises connection to serial port.

        Parameters
        ----------
        port : str
            Serial port of device
        baudrate : int, optional
            Baud rate for serial communication (default is 115200, STM32 functions at 115200)
        xonxoff  : bool, optional
            Flow control (default is on)
        """

        self.ser = serial.Serial(port, baudrate=baudrate, xonxoff=xonxoff, timeout=1)
        # Print serial port settings
        print("\nSerial Port Settings:")
        print("Port:", self.ser.port)
        print("Baudrate:", self.ser.baudrate)
        print("Byte size:", self.ser.bytesize)
        print("Parity:", self.ser.parity)
        print("Stop bits:", self.ser.stopbits)
        print("Timeout:", self.ser.timeout)
        print("Xon/Xoff:", self.ser.xonxoff)
        print("Rts/cts:", self.ser.rtscts)
        print("Dsr/dtr:", self.ser.dsrdtr)
        print("\n")

    def __del__(self):
        """Destructor

        Closes connection to serial port.
        """

        self.ser.close()

class SoilPowerSensorController(SerialController):
    """Controller used to read values from the SPS"""

    def __init__(self, port):
        """Constructor

        Opens serial connection and checks functionality

        Parameters
        ----------
        port : str
            Serial port of device
        """
        super().__init__(port)
        self.check()

    def get_phytos(self) -> Tuple[float, float]:
        """Measure phytos from SPS

        Returns
        -------
        tuple[float, float]
            voltage, current
        """
        
        self.ser.write(b"0\n") # send a command to the SPS to send a power measurment

        # read a single byte for the length
        resp_len_bytes = self.ser.read()
        resp_len = int.from_bytes(resp_len_bytes)

        reply = self.ser.read(resp_len) # read said measurment
        meas_dict = decode_measurement(reply) # decode using protobuf

        voltage_value = meas_dict["data"]["voltage"]
        leaf_value = meas_dict["data"]["leafWetness"]


        return float(voltage_value), float(leaf_value)

    def check(self):
        """Performs a check of the connection to the board

        Raises
        ------
        RuntimeError
            Checks that SPS replies "ok" when sent "check"
        """
        self.ser.write(b"check\n")
        reply = self.ser.readline()
        #reply = reply.decode()
        #reply = reply.strip("\r\n")
        if (reply != b'ok\n'):
            raise RuntimeError(f"SPS check failed. Reply received: {reply}")



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Data recorder for Soil Power Sensor via serial")
    parser.add_argument("port", type=str, help="Serial port of the Soil Power Sensor")
    parser.add_argument("interval", type=int, help="Logging interval in seconds")
    parser.add_argument("csv_file", type=str, help="CSV file to log the data")

    args = parser.parse_args()

    # Initialize the Soil Power Sensor controller
    sps = SoilPowerSensorController(args.port)

    try:
        while True:
            # Get measurement from the Soil Power Sensor
            voltage, leaf_wetness = sps.get_phytos()

            # Prepare data to log to CSV
            data = {"voltage": [voltage], "leaf wetness": [leaf_wetness], "timestamp": [time.time()]}
            df = pd.DataFrame(data)

            # Check if the CSV file exists; if not, create it with headers
            if not os.path.exists(args.csv_file):
                df.to_csv(args.csv_file, mode="w", header=True, index=False)
            else:
                # Append to CSV file
                df.to_csv(args.csv_file, mode="a", header=False, index=False)


            # Wait for the specified interval before the next measurement
            time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\nData logging interrupted.")
    finally:
        # Close the serial connection when done
        sps.serial.close()
