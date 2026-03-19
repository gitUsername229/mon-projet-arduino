#include "NfcManager.h"

// Initialize the raw pn532 hardware
NfcManager::NfcManager(HardwareSerial& serial) : pn532hsu(serial), pn532(pn532hsu), isEnabled(true) {}

void NfcManager::begin() {
  pn532.begin();
  pn532.SAMConfig(); // Secure Access Module config, necessary for normal reads
}

void NfcManager::enable() {
  isEnabled = true;
}

void NfcManager::disable() {
  isEnabled = false;
}

void NfcManager::resetHardware() {
  pn532.begin();     // Réveille la communication série du module
  pn532.SAMConfig(); // Reconfigure le mode de lecture de cartes
}

String NfcManager::readValidUid() {
  if (!isEnabled) {
    return "";
  }
  
  uint8_t uid[7];
  uint8_t uidLength;
  
  // Timeout in milliseconds to prevent stalling the entire microprocessor loop.
  // 30ms is usually more than enough to detect a passive target.
  if (pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 40)) {
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] <= 0x0F) uidStr += "0"; // Zero padding
        uidStr += String(uid[i], HEX);     // Read hexadecimal part
        if (i < uidLength - 1) uidStr += " "; // Original format separator
    }
    
    uidStr.toUpperCase(); // Ensure uppercase to match saved history format
    return uidStr;
  }
  
  return "";
}
