import os
import time
import random
import serial
import gpiozero
from gpiozero.pins.lgpio import LGPIOFactory
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device, "-", port.description)

factory = LGPIOFactory()

led = gpiozero.LED(21)
led.off()

servo = gpiozero.AngularServo(
    14, min_angle=0, max_angle=270, initial_angle=135,
    min_pulse_width=0.0006,  # try 600 µs
    max_pulse_width=0.0024,  # try 2400 µs
    pin_factory=factory
)
servo.angle = 0

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
        if message == "led":
            led.toggle()
        elif message.startswith("servo"):
            if len(message) > 5:
                servo.angle = int(message[5:])
            else:
                servo.angle = None
        elif message.startswith("lift"):
            print(f"Lifting to {int(message[4:])}%")
        elif message.startswith("drive"):
            msg = message.split(" ")
            dx = int(msg[1])
            dy = int(msg[2])
            rotation = int(msg[3])
            print(f"Driving x:{dx} y:{dy} rotation:{"clockwise" if rotation > 0 else "counterclockwise" if rotation < 0 else "none"}")
