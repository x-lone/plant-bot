import serial
import time
import os

port = os.getenv("SERIAL_PORT", "COM4")
baud = int(os.getenv("BAUD_RATE", "9600"))

ser = serial.Serial(port, baud, timeout=0.1)

while True:
    line = ser.readline().decode(errors='ignore').strip()
    if line == "READY":
        print("ARDUINO:", line)
        break

ser.reset_input_buffer()
ser.reset_output_buffer()

while True:
    line = ser.readline().decode(errors='ignore').strip()
    if line:
        print("ARDUINO:", line)

    cmd = input("> ").strip()

    if cmd == "":
        continue

    if cmd:
        ser.write((cmd + "\n").encode())

    time.sleep(0.05)