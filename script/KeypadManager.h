#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Arduino.h>

enum KeypadResult {
  KEYPAD_CONFIG_CODE = 1,
  KEYPAD_ACCESS_CODE = 0,
  KEYPAD_DIGIT_PRESSED = -1,
  KEYPAD_NO_KEY = -2
};

class KeypadManager {
private:
  String inputBuffer;
  String lastAccessCode;
  HardwareSerial& keypadSerial;
  
public:
  KeypadManager(HardwareSerial& serial);
  void begin();
  
  char readKey();
  void addKey(char key);
  bool checkConfigCode();
  bool checkAccessCode(String accessCode);
  void clear();
  String getBuffer() const;
  String getLastAccessCode() const;
  bool isValidationKey(char key) const;
  
  KeypadResult handleKeypad();
};

#endif
