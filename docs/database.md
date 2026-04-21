# Database Setup – Plant Bot
This project uses PostgreSQL to store sensor data over time.

---

## Database Creation

### 1. Create the Database
```sql
CREATE DATABASE plant_telemetry;
```

---

### 2. Create the Table
```sql
CREATE TABLE sensor_data (
    id SERIAL PRIMARY KEY,
    plant_name TEXT NOT NULL,
    temperature FLOAT,
    humidity FLOAT,
    soil FLOAT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```