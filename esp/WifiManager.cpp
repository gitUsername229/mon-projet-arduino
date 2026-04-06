#include "WifiManager.h"
#include <ArduinoJson.h>

WifiManager::WifiManager() : dataManager(nullptr), server(80), onUnlockCallback(nullptr) {}

void WifiManager::setupRoutes() {
  server.on("/config", HTTP_POST, [this]() {
    this->handleConfig();
  });
  
  server.onNotFound([this]() {
    server.send(404, "text/plain", "Not found");
  });
}

void WifiManager::handleConfig() {
  if (server.hasArg("plain") == false) { // Body is in "plain"
    server.send(400, "text/plain", "Body not received");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, jsonStr);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  if (doc.containsKey("accessCode")) {
      dataManager->updateAccessCode(doc["accessCode"].as<String>());
  }
  
  if (doc.containsKey("badges")) {
      JsonArray badgesArray = doc["badges"].as<JsonArray>();
      std::vector<String> newBadges;
      for(JsonVariant v : badgesArray) {
          newBadges.push_back(v.as<String>());
      }
      dataManager->setAuthorizedBadges(newBadges);
  }
  
  if (doc.containsKey("unlock") && doc["unlock"].as<bool>() == true) {
      if (onUnlockCallback != nullptr) {
          onUnlockCallback();
      }
  }
  
  server.send(200, "application/json", "{\"success\":true}");
}

void WifiManager::registerWithBackend() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + backendIp + ":" + String(backendPort) + "/api/esp/register";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(256);
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = WiFi.localIP().toString();
    String payload;
    serializeJson(doc, payload);
    
    int httpResponseCode = http.POST(payload);
    http.end();
  }
}

void WifiManager::sendHeartbeat() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + backendIp + ":" + String(backendPort) + "/api/esp/heartbeat";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(256);
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = WiFi.localIP().toString();
    String payload;
    serializeJson(doc, payload);
    
    http.POST(payload);
    http.end();
  }
}

void WifiManager::begin(DataManager& dm) {
  dataManager = &dm;
  
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - startAttemptTime >= 5000) {
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      startAttemptTime = millis();
    }
  }
  
  backendIp = WiFi.gatewayIP().toString();
  
  setupRoutes();
  server.begin();
  
  registerWithBackend();
}

void WifiManager::forceSync() {
  if (WiFi.status() != WL_CONNECTED) return;
  auto history = dataManager->getHistory();
  if (history.size() == 0) return;

  DynamicJsonDocument doc(2048);
  doc["type"] = "logs";
  doc["mac"] = WiFi.macAddress();
  JsonArray logsArray = doc.createNestedArray("data");
  
  for(size_t i = 0; i < history.size(); i++) {
      JsonObject logObj = logsArray.createNestedObject();
      logObj["uid"] = history[i].uid;
      logObj["success"] = history[i].success;
  }
  
  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  String url = "http://" + backendIp + ":" + String(backendPort) + "/api/esp/logs";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(payload);
  http.end();
  
  // Clear history if request was successful (200 OK)
  if (httpResponseCode >= 200 && httpResponseCode < 300) {
    dataManager->clearHistory();
  }
}

void WifiManager::handleLoop(void (*onUnlockCall)()) {
  onUnlockCallback = onUnlockCall;
  
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt >= 5000) {
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      lastReconnectAttempt = millis();
    }
    return;
  }

  server.handleClient();
  
  if (millis() - lastHeartbeatTime >= 5000) {
    lastHeartbeatTime = millis();
    sendHeartbeat();
  }
  
  // Check if there are logs to sync
  auto history = dataManager->getHistory();
  if (history.size() > 0) {
      forceSync();
  }
}
