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

int speedToDelay(int v) {
  // v=0 => slow, v=255 => fast
  return map(v, 0, 255, 200, 10);
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

  int delayMs = speedToDelay(speedValue);
  unsigned long now = millis();
  if (now - lastStep >= (unsigned long)delayMs) {
    lastStep = now;

    FastLED.clear();
    leds[head] = CRGB::Green;
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
