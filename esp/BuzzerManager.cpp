#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(uint8_t buzzerPin) : pin(buzzerPin) {}

void BuzzerManager::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void BuzzerManager::success() {
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
}

void BuzzerManager::error() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(50);
  }
}
