# Plant Bot Wiring Diagram
This document describes how all hardware components are connected to the Arduino.

---

## Components Used
- Arduino Uno (or compatible board)
- DHT11 Temperature & Humidity Sensor
- Soil Moisture Sensor
- 32x8 MAX7219 LED Matrix Display
- Push Button

---

## Pin Mapping

### DHT11 Sensor
- Data > Pin 9
- VCC > 5V
- GND > GND

---

### Soil Moisture Sensor
- Analog Output > A0
- VCC > 5V
- GND > GND

---

### Button
- Signal > Pin 12
- Other side > GND
- Uses internal pull-up resistor

---

### 32x8 LED Matrix (MAX7219)
- DIN > Pin 2
- CLK > Pin 4
- CS > Pin 3
- VCC > 5V
- GND > GND

---

## Power Notes
All components share a common ground. The Arduino is powered via USB or external 5V supply.