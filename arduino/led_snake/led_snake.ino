#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 60
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

int head = 0;
int dir = 1;
int speedValue = 80; // 0-255 from PC
unsigned long lastStep = 0;

const int stepSize = 8; // 1 length per 8 values
const int minDelay = 20;  // fastest
const int maxDelay = 200; // slowest

const uint8_t hueStart = 160; // blue
const uint8_t hueEnd = 0;     // red

int lengthFromValue(int v) {
  int step = v / stepSize;
  int maxLen = min(NUM_LEDS, 1 + (255 / stepSize));
  return constrain(1 + step, 1, maxLen);
}

int delayFromLength(int len) {
  int maxLen = min(NUM_LEDS, 1 + (255 / stepSize));
  return map(len, 1, maxLen, minDelay, maxDelay);
}

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    speedValue = Serial.read();
  }

  int snakeLen = lengthFromValue(speedValue);
  int delayMs = delayFromLength(snakeLen);
  uint8_t hue = map(speedValue, 0, 255, hueStart, hueEnd);
  CRGB color = CHSV(hue, 255, 255);
  unsigned long now = millis();
  if (now - lastStep >= (unsigned long)delayMs) {
    lastStep = now;

    FastLED.clear();
    for (int i = 0; i < snakeLen; i++) {
      int idx = head - dir * i;
      if (idx >= 0 && idx < NUM_LEDS) {
        leds[idx] = color;
      }
    }
    FastLED.show();

    head += dir;
    if (head >= NUM_LEDS - 1) {
      head = NUM_LEDS - 1;
      dir = -1;
    } else if (head <= 0) {
      head = 0;
      dir = 1;
    }
  }
}
