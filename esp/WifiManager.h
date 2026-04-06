#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <vector>
#include "DataManager.h"

class WifiManager {
private:
  DataManager* dataManager;
  const char* ssid = "casierPc";
  const char* password = "wificasierpc";
  WebServer server;
  
  String backendIp;
  int backendPort = 3000;
  unsigned long lastHeartbeatTime = 0;
  
  void (*onUnlockCallback)();

  void setupRoutes();
  void registerWithBackend();
  void sendHeartbeat();
  void handleConfig();

public:
  WifiManager();
  
  void begin(DataManager& dm);
  void handleLoop(void (*onUnlockCall)());
  
  // Immediatly sends offline logs and requests state
  void forceSync();
};

#endif
