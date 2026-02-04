import os
import time
import serial

import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device, "-", port.description)


ser = None
while not ser:
    try:
        if os.name == 'nt':
            ser = serial.Serial("COM5", 115200, timeout=1)  # Windows
        elif os.name == 'posix':
            ser = serial.Serial('/dev/ttyACM0', 115200)  # Linux / Pi
        else:
            raise Exception("Unsupported OS")
    except Exception as e:
        print(e, "\nRetrying in 10s...")
        time.sleep(10)

ser.write(b"connected\n")

while True:
    if ser.in_waiting:
        message = ser.readline().decode().strip()
        print(message)
