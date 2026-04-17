import os
import time
import serial
import psycopg2

INTERVAL = 300

PORT = os.getenv("SERIAL_PORT", "COM4")
BAUD = int(os.getenv("BAUD_RATE", "9600"))

ser = serial.Serial(PORT, BAUD, timeout=0.1)

conn = psycopg2.connect(
    host=os.getenv("DB_HOST", "localhost"),
    dbname=os.getenv("DB_NAME", "postgres"),
    user=os.getenv("DB_USER", "postgres"),
    password=os.getenv("DB_PASSWORD", ""),
    port=int(os.getenv("DB_PORT", "5432")),
)

temp_sum = hum_sum = soil_sum = count = 0

while True:
    line = ser.readline().decode(errors='ignore').strip()
    if line == "READY":
        print("ARDUINO:", line)
        break

ser.reset_input_buffer()
ser.reset_output_buffer()

start_time = time.time()

while True:
    try:
        line = ser.readline().decode(errors='ignore').strip()
    except Exception as e:
        print("SERIAL_ERROR:", e)
        time.sleep(2)
        continue

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

    if time.time() - start_time >= INTERVAL:
        start_time = time.time()

        if count > 0:
            try:
                avg_temp = round(temp_sum / count, 2)
                avg_hum  = round(hum_sum / count, 2)
                avg_soil = round(soil_sum / count, 2)

                with conn.cursor() as cur:
                    cur.execute(
                        "INSERT INTO sensor_data (temperature, humidity, soil) VALUES (%s, %s, %s)",
                        (avg_temp, avg_hum, avg_soil)
                    )
                    conn.commit()

                    print(f"INSERTED:{avg_temp},{avg_hum},{avg_soil}")
                    
            except Exception as e:
                print("DB_ERROR:", e)
                conn.rollback()

        temp_sum = hum_sum = soil_sum = count = 0

    time.sleep(0.05)