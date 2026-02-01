import time
import serial

ser = None
while not ser:
    try:
        ser = serial.Serial("COM5", 115200, timeout=1)  # Windows
        # ser = serial.Serial('/dev/ttyACM0', 115200)  # Linux / Pi
    except Exception as e:
        print(e, "\nRetrying in 10s...")
        time.sleep(10)

ser.write(b"connected\n")

while True:
    if ser.in_waiting:
        message = ser.readline().decode().strip()
        print(message)
