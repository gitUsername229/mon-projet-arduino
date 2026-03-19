#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include "DataManager.h"
#include "page_web.h"

class WebManager {
private:
  WebServer server;
  DataManager* dataManager;

  void handleLogin();
  void handleRoot();
  void handleToggleBadge();
  void handlePassword();
  void handleAccessCode();
  void handleLogout();
  
  bool isAuthenticated();

public:
  WebManager();

  void begin(DataManager& dm, const char* ssid);
  void stop();
  void handleClient();
};

#endif
