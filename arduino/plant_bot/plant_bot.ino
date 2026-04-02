#include <MD_MAX72xx.h>
#include <SPI.h>
#include "DHT.h"
#include <string.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3
#define CLK_PIN 4
#define DIN_PIN 2

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILPIN A0

char buffer[64];
byte index = 0;

void handleCommand(char *cmd) {
  while (*cmd == ' ') cmd++;

  if (strcmp(cmd, "GET") == 0) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int soil = analogRead(SOILPIN);

    if (isnan(temp) || isnan(hum)) {
      Serial.println("ERR,DHT_FAIL");
      return;
    }

    Serial.print(temp);
    Serial.print(",");
    Serial.print(hum);
    Serial.print(",");
    Serial.println(soil);
  }
  else if (strcmp(cmd, "DRAW") == 0) {
    if (matrix.getPoint(1,1)) {
      matrix.setPoint(1, 1, false);
      Serial.println("OK,DRAW,MATRIX(1,1)OFF");
    } else {
      matrix.setPoint(1, 1, true);
      Serial.println("OK,DRAW,MATRIX(1,1)ON");
    }
  }
  else if (strncmp(cmd, "TEXT ", 5) == 0) {
    char *msg = cmd + 5;

    Serial.print("OK,TEXT,");
    Serial.println(msg);
  }
  else {
    Serial.print("ERR,UNKNOWN_CMD,");
    Serial.println(cmd);
  }
}

void setup() {
  Serial.begin(9600);

  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 5);
  matrix.clear();

  dht.begin();

  Serial.println("READY");
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (index == 0) continue;
      buffer[index] = '\0';
      handleCommand(buffer);
      index = 0;
    }
    else {
      if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      }
      else {
        index = 0;
      }
    }
  }
}