#ifndef NFC_MANAGER_H
#define NFC_MANAGER_H

#include <NfcAdapter.h> // Forces Arduino IDE dependency resolver to locate PN532 dependencies
#include <PN532/PN532/PN532.h>
#include <PN532/PN532_HSU/PN532_HSU.h>
#include <Arduino.h>

class NfcManager {
private:
  PN532_HSU pn532hsu;
  PN532 pn532;
  bool isEnabled;
  
public:
  NfcManager(HardwareSerial& serial);
  
  void begin();
  void enable();
  void disable();
  void resetHardware();
  
  // Scans for a tag and returns its UID as string if found, otherwise empty string.
  String readValidUid();
};

#endif
