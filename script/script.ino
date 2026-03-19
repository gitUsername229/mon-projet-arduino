#include "FeatherShieldESP32Pinouts.h"
#include <ESP32Servo.h>
#include <NfcAdapter.h> // Keep here to force Arduino IDE to discover the NDEF library path
#include <PN532/PN532/PN532.h>
#include <PN532/PN532_HSU/PN532_HSU.h>
#include <ChainableLED.h> 
#include <WiFi.h>
#include <Preferences.h>
#include <vector>

#include "LedManager.h"
#include "BuzzerManager.h"
#include "ServoManager.h"
#include "NfcManager.h"
#include "DataManager.h"
#include "KeypadManager.h"
#include "WebManager.h"

const char* WIFI_SSID = "CASIER_ESP32";
const int BUTTON_PIN = A2;

LedManager ledManager(D2, D3, 1);
BuzzerManager buzzerManager(D4);
ServoManager servoManager(A0);
NfcManager nfcManager(Serial1);
DataManager dataManager;
KeypadManager keypadManager(Serial2);
WebManager webManager;

enum SystemMode {
  MODE_NORMAL,
  MODE_CONFIG
};

SystemMode currentMode = MODE_NORMAL;

void unlockSafe() {
  servoManager.unlock();
  ledManager.ok();
  buzzerManager.success();
  delay(700);
  ledManager.off();
  delay(1000);
  nfcManager.resetHardware(); // Récupère le PN532 qui perd son SAMConfig quand le servo tire le courant
}

void rejectAccess() {
  ledManager.err();
  buzzerManager.error();
  delay(500);
  ledManager.off();
  delay(1000);
  nfcManager.resetHardware();
}

void lockSafe() {
  servoManager.lock();
  ledManager.off();
  buzzerManager.success();
  delay(200);

  if (currentMode == MODE_CONFIG) {
    ledManager.config();
  } else {
    ledManager.off();
  }
  nfcManager.resetHardware(); // Récupère le PN532 qui perd son SAMConfig
}

void enterConfigMode() {
  if (!servoManager.isLocked()) {
    return;
  }
  
  currentMode = MODE_CONFIG;
  ledManager.config();
  nfcManager.disable();
  webManager.begin(dataManager, WIFI_SSID);
}

void exitConfigMode() {
  currentMode = MODE_NORMAL;
  webManager.stop();
  ledManager.off();
  nfcManager.enable();
  nfcManager.resetHardware(); // Réinitialise matériellement la puce NFC après les grosses variations de tension du WiFi
}

void handleNormalMode(KeypadResult keypadResult) {
  if (keypadResult == KEYPAD_CONFIG_CODE) {
    enterConfigMode();
    return;
  }
  
  if (servoManager.isLocked()) {
    if (keypadResult == KEYPAD_ACCESS_CODE) {
      String expectedCode = dataManager.getAccessCode();
      String enteredCode = keypadManager.getLastAccessCode();
      
      if (enteredCode == expectedCode) {
        unlockSafe();
      } else {
        rejectAccess();
      }
    } 
    else {
      String uid = nfcManager.readValidUid();
      if (uid.length() > 0) {
        bool authorized = dataManager.isAuthorized(uid);
        dataManager.addLog(uid, authorized);
        
        if (authorized) {
          unlockSafe();
        } else {
          rejectAccess();
        }
      }
    }
  }
  else {
    int buttonRaw = analogRead(BUTTON_PIN);
    if (buttonRaw > 1000) {
      lockSafe();
    }
  }
}

void handleConfigMode(KeypadResult keypadResult) {
  static unsigned long lastWebServer = 0;
  if (millis() - lastWebServer > 100) {
    webManager.handleClient();
    lastWebServer = millis();
  }
  
  if (keypadResult == KEYPAD_CONFIG_CODE) {
    exitConfigMode();
    return;
  }
  
  if (servoManager.isUnlocked()) {
    int buttonRaw = analogRead(BUTTON_PIN);
    if (buttonRaw > 1000) {
      lockSafe();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, A4, -1);
  
  WiFi.mode(WIFI_OFF);
  
  ledManager.begin();
  buzzerManager.begin();
  servoManager.begin();
  nfcManager.begin();
  dataManager.begin();
  keypadManager.begin();
  
  currentMode = MODE_NORMAL;
  ledManager.off();
  nfcManager.enable();
}

void loop() {
  KeypadResult keypadResult = KEYPAD_NO_KEY;
  for(int i = 0; i < 5; i++) { 
    keypadResult = keypadManager.handleKeypad();
    if(keypadResult != KEYPAD_NO_KEY) break;
    delay(1);
  }

  if (keypadResult == KEYPAD_DIGIT_PRESSED) {
    buzzerManager.success();
  }

  if (currentMode == MODE_NORMAL) {
    handleNormalMode(keypadResult);
  } else if (currentMode == MODE_CONFIG) {
    handleConfigMode(keypadResult);
  }
}