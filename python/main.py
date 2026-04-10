import serial
import time
import os

port = os.getenv("SERIAL_PORT", "COM4")
baud = int(os.getenv("BAUD_RATE", "9600"))

ser = serial.Serial(port, baud, timeout=0.1)

start_time = time.time()

temp_sum = hum_sum = soil_sum = count = 0

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
        try:
            protocol, payload = line.split(":", 1)

            if protocol == "SENSOR_DATA":
                temp, hum, soil = map(float, payload.split(","))

                temp_sum += temp
                hum_sum += hum
                soil_sum += soil
                count += 1

        except ValueError:
            continue

    if time.time() - start_time >= 10:
        start_time = time.time()

        if count > 0:
            temp_sum /= count
            hum_sum /= count
            soil_sum /= count

            print(temp_sum, hum_sum, soil_sum)

        temp_sum = hum_sum = soil_sum = count = 0

    time.sleep(0.05)