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

byte A[6] = {0b00000000, 0b01000010, 0b00001000, 0b10000001, 0b01111110, 0b00000000};

#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILPIN A0

char buffer[64];
byte index = 0;

int matrixPixelOffset(int size, int pos, bool flip) {
  return flip ? (size - 1 - pos) : pos;
}

void drawBitmap(byte *image, int image_width, int image_height, int xPos, int yPos) {
  for (int row = 0; row < image_height; row++) {
    for (int col = 0; col < image_width; col++) {
      bool pixel = bitRead(image[row], col);

      int x = matrixPixelOffset(image_width, col, true) + xPos;
      int y = matrixPixelOffset(image_height, row, true) + yPos;

      matrix.setPoint(y, x, pixel);
    }
  }
}

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
    if (!matrix.getPoint(0,0)) {
      // drawBitmap(A, 3, 3, 0, 0);
    } else {
      matrix.clear();
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
  drawBitmap(A, 8, 6, 0, 0);

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