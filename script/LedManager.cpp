#include "LedManager.h"

LedManager::LedManager(uint8_t pin1, uint8_t pin2, uint8_t numLeds) : leds(pin1, pin2, numLeds) {}

void LedManager::begin() {
  leds.setColorRGB(0, 0, 0, 0);
}

void LedManager::config() {
  leds.setColorRGB(0, 255, 255, 0);
}

void LedManager::ok() {
  leds.setColorRGB(0, 0, 255, 0);
}

void LedManager::err() {
  leds.setColorRGB(0, 255, 0, 0);
}

void LedManager::off() {
  leds.setColorRGB(0, 0, 0, 0);
}
