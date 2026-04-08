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

uint16_t A[6] = {0b00000000, 0b01000010, 0b00001000, 0b10000001, 0b01111110, 0b00000000};
uint16_t temp[3] = {0b1110111011101110, 0b0100110011101110, 0b0100111010101000};
uint16_t humi[3] = {0b1010101011101110, 0b1110101011100100, 0b1010111010101110};
uint16_t soil[3] = {0b0110111011101000, 0b0100101001001000, 0b1100111011101110};

#define DIGIT_W 3
#define DIGIT_H 4
#define DIGIT_SPACING (DIGIT_W + 1)
uint16_t digit_bitmaps[10][4] {
  {0b111, 0b101, 0b101, 0b111},
  {0b110, 0b010, 0b010, 0b111},
  {0b111, 0b001, 0b110, 0b111},
  {0b111, 0b011, 0b001, 0b111},
  {0b101, 0b101, 0b111, 0b001},
  {0b111, 0b100, 0b011, 0b111},
  {0b100, 0b111, 0b101, 0b111},
  {0b111, 0b001, 0b010, 0b010},
  {0b111, 0b101, 0b111, 0b111},
  {0b111, 0b101, 0b111, 0b001}
};


#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILPIN A0

char buffer[64];
uint16_t bufIndex = 0;

int matrixPixelOffset(int size, int pos, bool flip) {
  return flip ? (size - 1 - pos) : pos;
}

void drawBitmap(uint16_t *image, int image_width, int image_height, int xPos, int yPos) {
  for (int row = 0; row < image_height; row++) {
    for (int col = 0; col < image_width; col++) {
      bool pixel = bitRead(image[row], col);

      int x = matrixPixelOffset(image_width, col, true) + xPos;
      int y = matrixPixelOffset(image_height, row, true) + yPos;

      matrix.setPoint(y, x, pixel);
    }
  }
}

void sendSensorValues() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = analogRead(SOILPIN);

  if (isnan(temp) || isnan(hum)) {
    Serial.println("ERR,DHT_FAIL");
    return;
  }

  Serial.print("RETURNED:");
  Serial.print(temp);
  Serial.print(",");
  Serial.print(hum);
  Serial.print(",");
  Serial.println(soil);
}

void drawNumber(char *num) {
  int x = 0;

  while (*num) {
    if (*num >= '0' && *num <= '9') {
      drawBitmap(digit_bitmaps[*num - '0'], DIGIT_W, DIGIT_H, x, 0);
      x += DIGIT_SPACING;
    }
    else if (*num == '.'){
      matrix.setPoint(0, x, true);
      x += 2;
    }

    num++;
  }
}

void handleCommand(char *cmd) {
  while (*cmd == ' ') cmd++;

  if (strcmp(cmd, "GET") == 0) {
    sendSensorValues();
  }
  else if (strncmp(cmd, "DISPLAY:", 8) == 0) {
    matrix.clear();

    if (strncmp(cmd + 8, "TEMP:", 5) == 0) {
      drawBitmap(temp, 16, 3, 0, 5);
    }
    else if (strncmp(cmd + 8, "HUMI:", 5) == 0) {
      drawBitmap(humi, 16, 3, 0, 5);
    }
    else if (strncmp(cmd + 8, "SOIL:", 5) == 0) {
      drawBitmap(soil, 16, 3, 0, 5);
    }

    drawNumber(cmd + 13);
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
      if (bufIndex == 0) continue;
      buffer[bufIndex] = '\0';
      handleCommand(buffer);
      bufIndex = 0;
    }
    else {
      if (bufIndex < sizeof(buffer) - 1) {
        buffer[bufIndex++] = c;
      }
      else {
        buffer[0] = '\0';
        bufIndex = 0;
      }
    }
  }
}