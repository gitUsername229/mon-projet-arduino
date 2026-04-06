#include "ServoManager.h"

ServoManager::ServoManager(uint8_t pin) : locked(true), servoPin(pin) {}

void ServoManager::begin() {
  ESP32PWM::allocateTimer(0);
  servo.setPeriodHertz(50);
  servo.attach(servoPin, 500, 2400);
  lock();
}

void ServoManager::lock() {
  servo.write(0);
  locked = true;
}

void ServoManager::unlock() {
  servo.write(75);
  locked = false;
}

bool ServoManager::isLocked() const {
  return locked;
}

bool ServoManager::isUnlocked() const {
  return !locked;
}
