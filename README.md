# Plant Bot – IoT Monitoring System
Plant Bot is a IoT-based monitoring system that collects real-time sensor data using an Arduino, processes it in Python, stores it in PostgreSQL, and visualizes it with a Flask web dashboard.

---

## Features
- Real-time temperature, humidity, and soil monitoring
- LED matrix sensor display
- Button-controlled sensor switching (TEMP / HUM / SOIL)
- PostgreSQL time-series storage
- Flask web dashboard with Chart.js graphs

---

## System Architecture
1. Arduino sensors generate readings
2. Python serial script collects and averages data
3. Data is stored in PostgreSQL database
4. Flask API serves data to frontend
5. Chart.js renders live and historical graphs

---

## Hardware Setup
See full wiring diagram here:

See `docs/wiring.md`

---

## Software Requirements

### Python Dependencies
Install using:

```bash
pip install -r python/requirements.txt
```

---

## Environment Variables
These must be set before running the project:
- DB_HOST
- DB_NAME
- DB_USER
- DB_PASSWORD
- DB_PORT
- SERIAL_PORT
- BAUD_RATE

---

## Database Setup
Database setup instructions are located here:

See `docs/database.md`

---

## Running the Project
Start services in order: Arduino > Database > Serial Script > Flask Server

### 1. Upload Arduino Code
1. Open Arduino IDE
2. Open arduino/plant_bot.ino
3. Select correct board + port
4. Upload to device

---

### 2. Start PostgreSQL Database
Ensure PostgreSQL is running and the database is created.

---

### 3. Set Environment Variables
Make sure all required environment variables are set before running Python scripts.

---

### 4. Start Serial Data Collector
```bash
python python/serial_interface.py
```

---

### 5. Start Flask Web Dashboard
```bash
python python/app.py
```

---

### 6. Open Dashboard
Visit `http://localhost:5000`

---

## Notes
- Ensure Arduino is connected before starting the serial script  
- Ensure PostgreSQL is running before launching Python services  
- Data updates every 5 minutes  