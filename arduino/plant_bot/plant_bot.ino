#include <MD_MAX72xx.h>
#include <SPI.h>
#include "DHT.h"

#define BTNPIN 12
const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;
byte lastButtonState = HIGH;
byte buttonState = HIGH;

#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILPIN A0

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3
#define CLK_PIN 4
#define DIN_PIN 2

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const byte A[6] PROGMEM = {0b00000000, 0b01000010, 0b00001000, 0b10000001, 0b01111110, 0b00000000};

#define DIGIT_W 3
#define DIGIT_H 4
#define DIGIT_SPACING (DIGIT_W + 1)
const byte digit_bitmaps[10][4] PROGMEM = {
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
const byte letter_bitmaps [26][3] PROGMEM = {
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

const unsigned long interval = 1000;
unsigned long lastUpdate = 0;

enum SensorType { TEMP, HUM, SOIL };
byte current_sensor = TEMP;
char current_value[12];

float getTemperature() {
  for (int i = 0; i < 3; i++) {
    float temp = dht.readTemperature();
    if (!isnan(temp)) return temp;
  }

  return -999;
}

int getHumidity() {
  for (int i = 0; i < 3; i++) {
    float hum = dht.readHumidity();
    if (!isnan(hum)) return hum;
  }

  return -1;
}

int getSoil() {
  return analogRead(SOILPIN);
}

int pixelOffset(int size, int pos, bool flip) {
  return flip ? (size - 1 - pos) : pos;
}

void drawBitmap(const byte *image, int image_width, int image_height, int xPos, int yPos) {
  for (int row = 0; row < image_height; row++) {
    for (int col = 0; col < image_width; col++) {
      bool pixel = (pgm_read_byte(&image[row]) >> col) & 0x01;

      int x = pixelOffset(image_width, col, true) + xPos;
      int y = pixelOffset(image_height, row, true) + yPos;

      matrix.setPoint(y, x, pixel);
    }
  }
}

void drawNumber(const char *num, int x, int y) {
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

void drawWord(const char *let, int x, int y) {
  int xOff = x;

  while (*let) {
    if (*let >= 'A' && *let <= 'Z') {
      drawBitmap(letter_bitmaps[*let - 'A'], LETTER_W, LETTER_H, xOff, y);
      xOff += LETTER_SPACING;
    }

    let++;
  }
}

void setup() {
  Serial.begin(9600);

  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 1);
  matrix.clear();
  drawBitmap(A, 8, 6, 0, 0);

  pinMode(BTNPIN, INPUT_PULLUP);

  dht.begin();

  Serial.println("READY");
}

void handleButton() {
  int reading = digitalRead(BTNPIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        current_sensor = (current_sensor + 1) % 3;

        lastUpdate = millis();
        update();
        draw();
      }
    }
  }

  lastButtonState = reading;
}

void update() {
  float temp = getTemperature();
  int hum = getHumidity();
  int soil = getSoil();

  switch (current_sensor) {
    case TEMP:
      dtostrf(temp, 4, 2, current_value);
      break;
    case HUM:
      itoa(hum, current_value, 10);
      break;
    case SOIL:
      itoa(soil, current_value, 10);
      break;
  }

  if (temp != -999 && hum != -1) {
    Serial.print("SENSOR_DATA:");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(hum);
    Serial.print(",");
    Serial.println(soil); 
  }
}

void draw() {
  matrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  matrix.clear();

  switch (current_sensor) {
    case TEMP: drawWord("TEMP", 0, 5); break;
    case HUM: drawWord("HUM", 0, 5); break;
    case SOIL: drawWord("SOIL", 0, 5); break;
  }

  drawNumber(current_value, 0, 0);

  matrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void loop() {
  handleButton();

  if (millis() - lastUpdate >= interval) {
    lastUpdate = millis();

    update();
    draw();
  }
}