#ifndef SERVO_MANAGER_H
#define SERVO_MANAGER_H

#include <ESP32Servo.h>

class ServoManager {
private:
  Servo servo;
  bool locked;
  uint8_t servoPin;
  
public:
  ServoManager(uint8_t pin);
  
  void begin();
  void lock();
  void unlock();
  
  bool isLocked() const;
  bool isUnlocked() const;
};

#endif
