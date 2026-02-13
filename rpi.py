import os
import time
import random
import serial
import gpiozero
import threading
from gpiozero import PWMOutputDevice, OutputDevice
from gpiozero.pins.lgpio import LGPIOFactory
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device, "-", port.description)

factory = LGPIOFactory()

led = gpiozero.LED(14)
led.off()

servo = gpiozero.AngularServo(
    21,
    min_angle=0,
    max_angle=270,
    initial_angle=135,
    min_pulse_width=0.0006,  # try 600 µs
    max_pulse_width=0.0024,  # try 2400 µs
    pin_factory=factory,
)
servo.angle = 0

step = OutputDevice(24)
direction = OutputDevice(23)
enable = OutputDevice(25)
enable.off()

STEP_DELAY = 0.001

# front left
fl_in1 = OutputDevice(6)
fl_in2 = OutputDevice(5)
pwm_fl = PWMOutputDevice(0)

# front right
fr_in1 = OutputDevice(4)
fr_in2 = OutputDevice(3)
pwm_fr = PWMOutputDevice(2)

# rear left
# rl_in1 = OutputDevice()
# rl_in2 = OutputDevice()
# pwm_rl = PWMOutputDevice()

# rear right
# rr_in1 = OutputDevice()
# rr_in2 = OutputDevice()
# pwm_rr = PWMOutputDevice()

# standby left
STBYL = OutputDevice(13)
STBYL.on()

# standby right
STBYR = OutputDevice(17)
STBYR.on()


def drive_motor(pwm, in1, in2, value):
    if value > 0:
        in1.on()
        in2.off()
        pwm.value = min(abs(value) / 300, 1.0)
    elif value < 0:
        in1.off()
        in2.on()
        pwm.value = min(abs(value) / 300, 1.0)
    else:
        in1.off()
        in2.off()
        pwm.value = 0


ser = None
while not ser:
    try:
        if os.name == "nt":
            ser = serial.Serial("COM5", 115200, timeout=1)  # Windows
        elif os.name == "posix":
            ser = serial.Serial("/dev/ttyACM0", 115200)  # Linux / Pi
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
            d = int(message[4:])
            if d > 0:
                direction.on()
                step.on()
                time.sleep(STEP_DELAY)
                step.off()
                time.sleep(STEP_DELAY)
            else:
                direction.off()
                step.on()
                time.sleep(STEP_DELAY)
                step.off()
                time.sleep(STEP_DELAY)
        elif message.startswith("drive"):
            msg = message.split(" ")
            x = int(msg[1])
            y = int(msg[2])
            rotation = int(
                msg[3]
            )  # -1 (counterclockwise), 0 (no rotation) or 1 (clockwise)
            r_scaled = rotation * 120
            fl = y + x + r_scaled
            fr = y - x - r_scaled
            rl = y - x + r_scaled
            rr = y + x - r_scaled

            # normalize
            max_val = max(abs(fl), abs(fr), abs(rl), abs(rr), 1)

            fl /= max_val
            fr /= max_val
            rl /= max_val
            rr /= max_val

            fl *= 300
            fr *= 300
            rl *= 300
            rr *= 300

            drive_motor(pwm_fl, fl_in1, fl_in2, fl)
            drive_motor(pwm_fr, fr_in1, fr_in2, fr)
            # drive_motor(pwm_rl, rl_in1, rl_in2, rl)
            # drive_motor(pwm_rr, rr_in1, rr_in2, rr)
