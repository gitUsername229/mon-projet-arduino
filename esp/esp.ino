#include "FeatherShieldESP32Pinouts.h"
#include <ESP32Servo.h>
#include <NfcAdapter.h>
#include <PN532/PN532/PN532.h>
#include <PN532/PN532_HSU/PN532_HSU.h>
#include <ChainableLED.h> 
#include <Preferences.h>
#include <vector>

#include "LedManager.h"
#include "BuzzerManager.h"
#include "ServoManager.h"
#include "NfcManager.h"
#include "DataManager.h"
#include "KeypadManager.h"
#include "WifiManager.h"

const int BUTTON_PIN = A2;

LedManager ledManager(D2, D3, 1);
BuzzerManager buzzerManager(D4);
ServoManager servoManager(A0);
NfcManager nfcManager(Serial1);
DataManager dataManager;
KeypadManager keypadManager(Serial2);
WifiManager wifiManager;

void unlockSafe() {
  servoManager.unlock();
  ledManager.ok();
  buzzerManager.success();
  delay(700);
  ledManager.off();
  delay(1000);
  nfcManager.resetHardware(); 
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
  ledManager.off();
  nfcManager.resetHardware(); 
}

void onRemoteUnlock() {
  unlockSafe();
}

void handleNormalMode(KeypadResult keypadResult) {
  if (servoManager.isLocked()) {
    if (keypadResult == KEYPAD_ACCESS_CODE) {
      String expectedCode = dataManager.getAccessCode();
      String enteredCode = keypadManager.getLastAccessCode();
      if (enteredCode == expectedCode) {
        dataManager.addLog("KEYPAD", true);
        unlockSafe();
      } else {
        dataManager.addLog("KEYPAD", false);
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

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, A4, -1);
  
  ledManager.begin();
  buzzerManager.begin();
  servoManager.begin();
  nfcManager.begin();
  dataManager.begin();
  keypadManager.begin();
  
  wifiManager.begin(dataManager);
  
  ledManager.off();
  nfcManager.enable();
}

void loop() {
  KeypadResult keypadResult = keypadManager.handleKeypad();

  if (keypadResult == KEYPAD_DIGIT_PRESSED) {
    buzzerManager.success();
  }

  handleNormalMode(keypadResult);
  wifiManager.handleLoop(onRemoteUnlock);
}