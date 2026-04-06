#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <Arduino.h>

class BuzzerManager {
private:
  uint8_t pin;
  
public:
  BuzzerManager(uint8_t buzzerPin);
  
  void begin();
  void success();
  void error();
};

#endif
