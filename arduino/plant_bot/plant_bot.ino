#include <MD_MAX72xx.h>
#include <SPI.h>
#include "DHT.h"
#include <string.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3
#define CLK_PIN 4
#define DIN_PIN 2

#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILPIN A0

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

byte A[6] = {0b00000000, 0b01000010, 0b00001000, 0b10000001, 0b01111110, 0b00000000};

#define DIGIT_W 3
#define DIGIT_H 4
#define DIGIT_SPACING (DIGIT_W + 1)
byte digit_bitmaps[10][4] {
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

#define LETTER_W 3
#define LETTER_H 3
#define LETTER_SPACING (LETTER_W + 1)
byte letter_bitmaps[26][3] {
  {0b010, 0b111, 0b101},
  {0b110, 0b111, 0b111},
  {0b111, 0b100, 0b111},
  {0b110, 0b101, 0b110},
  {0b111, 0b110, 0b111},
  {0b111, 0b110, 0b100},
  {0b110, 0b101, 0b111},
  {0b101, 0b111, 0b101},
  {0b111, 0b010, 0b111},
  {0b001, 0b101, 0b111},
  {0b101, 0b110, 0b101},
  {0b100, 0b100, 0b111},
  {0b111, 0b111, 0b101},
  {0b111, 0b101, 0b101},
  {0b111, 0b101, 0b111},
  {0b111, 0b111, 0b100},
  {0b111, 0b111, 0b001},
  {0b111, 0b100, 0b100},
  {0b011, 0b010, 0b110},
  {0b111, 0b010, 0b010},
  {0b101, 0b101, 0b111},
  {0b101, 0b101, 0b010},
  {0b101, 0b111, 0b111},
  {0b101, 0b010, 0b101},
  {0b101, 0b010, 0b010},
  {0b110, 0b010, 0b011}
};

char current_sensor[4] = "TEMP"; 
char current_value[12];

int pixelOffset(int size, int pos, bool flip) {
  return flip ? (size - 1 - pos) : pos;
}

void drawBitmap(byte *image, int image_width, int image_height, int xPos, int yPos) {
  for (int row = 0; row < image_height; row++) {
    for (int col = 0; col < image_width; col++) {
      bool pixel = bitRead(image[row], col);

      int x = pixelOffset(image_width, col, true) + xPos;
      int y = pixelOffset(image_height, row, true) + yPos;

      matrix.setPoint(y, x, pixel);
    }
  }
}

void drawNumber(char *num, int x, int y) {
  int xOff = x;

  while (*num) {
    if (*num >= '0' && *num <= '9') {
      drawBitmap(digit_bitmaps[*num - '0'], DIGIT_W, DIGIT_H, xOff, y);
      xOff += DIGIT_SPACING;
    }
    else if (*num == '.'){
      matrix.setPoint(y, xOff, true);
      xOff += 2;
    }

    num++;
  }
}

void drawWord(char *let, int x, int y) {
  int xOff = x;

  while (*let) {
    if (*let >= 'A' && *let <= 'Z') {
      drawBitmap(letter_bitmaps[*let - 'A'], LETTER_W, LETTER_H, xOff, y);
      xOff += LETTER_SPACING;
    }

    let++;
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

void setup() {
  Serial.begin(9600);

  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 1);
  matrix.clear();
  drawBitmap(A, 8, 6, 0, 0);

  dht.begin();

  Serial.println("READY");
}

void update() {
  dtostrf(dht.readTemperature(), 4, 2, current_value);
}

void draw() {
  matrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  matrix.clear();

  drawWord(current_sensor, 0, 5);
  drawNumber(current_value, 0, 0);

  matrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void loop() {
  update();
  draw();

  delay(1000);
}