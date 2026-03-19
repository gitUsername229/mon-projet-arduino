#include "KeypadManager.h"

const String CONFIG_CODE = "9999";

KeypadManager::KeypadManager(HardwareSerial& serial) : inputBuffer(""), keypadSerial(serial) {}

void KeypadManager::begin() {
}

char KeypadManager::readKey() {
  char key = '\0';
  
  if (keypadSerial.available()) {
    while(keypadSerial.available()) {
      uint8_t data = keypadSerial.read();
      
      switch(data) {
        case 0xE1: key = '1'; break;
        case 0xE2: key = '2'; break;
        case 0xE3: key = '3'; break;
        case 0xE4: key = '4'; break;
        case 0xE5: key = '5'; break;
        case 0xE6: key = '6'; break;
        case 0xE7: key = '7'; break;
        case 0xE8: key = '8'; break;
        case 0xE9: key = '9'; break;
        case 0xEB: key = '0'; break;
        case 0xEA: key = '*'; break;
        case 0xEC: key = '#'; break;
        default: 
          key = '?'; 
          break;
      }
    }
  }
  
  return key;
}

void KeypadManager::addKey(char key) {
  inputBuffer += key;
}

bool KeypadManager::checkConfigCode() {
  return inputBuffer == CONFIG_CODE;
}

bool KeypadManager::checkAccessCode(String accessCode) {
  return inputBuffer == accessCode;
}

void KeypadManager::clear() {
  inputBuffer = "";
}

String KeypadManager::getBuffer() const {
  return inputBuffer;
}

bool KeypadManager::isValidationKey(char key) const {
  return key == '#' || key == '*';
}

KeypadResult KeypadManager::handleKeypad() {
  char key = readKey();
  
  if (key != '\0') {
    if (isValidationKey(key)) {
      String buffer = getBuffer();
      
      if (buffer.length() > 0) {
        if (checkConfigCode()) {
          clear();
          return KEYPAD_CONFIG_CODE;
        } else {
          lastAccessCode = buffer;
          clear();
          return KEYPAD_ACCESS_CODE;
        }
      } else {
        clear();
        return KEYPAD_NO_KEY;
      }
    } else if (key != '?') {
      addKey(key);
      return KEYPAD_DIGIT_PRESSED;
    }
  }
  
  return KEYPAD_NO_KEY;
}

String KeypadManager::getLastAccessCode() const {
  return lastAccessCode;
}
