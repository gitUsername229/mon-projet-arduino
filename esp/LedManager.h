#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <ChainableLED.h>

class LedManager {
private:
  ChainableLED leds;
  
public:
  LedManager(uint8_t pin1, uint8_t pin2, uint8_t numLeds);
  
  void begin();
  void config();
  void ok();
  void err();
  void off();
};

#endif
