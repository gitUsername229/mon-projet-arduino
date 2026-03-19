#include "WebManager.h"
#include <Arduino.h>

WebManager::WebManager() : server(80), dataManager(nullptr) {
}

void WebManager::begin(DataManager& dm, const char* ssid) {
  dataManager = &dm;

  WiFi.setTxPower(WIFI_POWER_2dBm);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, "", 1, 0, 1);

  delay(1000); 

  // Configurer de l'extraction des cookies (indispensable pour l'auth)
  const char * headerkeys[] = {"Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize);

  server.on("/", [this]() { this->handleRoot(); });
  server.on("/login", [this]() { this->handleLogin(); });
  server.on("/logout", [this]() { this->handleLogout(); });
  server.on("/toggle", [this]() { this->handleToggleBadge(); });
  server.on("/pwd", HTTP_POST, [this]() { this->handlePassword(); });
  server.on("/accesscode", HTTP_POST, [this]() { this->handleAccessCode(); });
  server.begin();
}

void WebManager::stop() {
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
}

void WebManager::handleClient() {
  server.handleClient();
}

bool WebManager::isAuthenticated() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf("session=casier") != -1) {
      return true;
    }
  }
  return false;
}

void WebManager::handleLogin() {
  String errorMsg = "";
  
  if (server.method() == HTTP_POST && server.hasArg("password")) {
    String password = server.arg("password");
    if (password == dataManager->getAdminPass()) {
      server.sendHeader("Set-Cookie", "session=casier; Path=/; HttpOnly; Max-Age=3600");
      server.sendHeader("Location", "/");
      server.send(303);
      return;
    } else {
      errorMsg = "<div class='error'>Mot de passe incorrect</div>";
    }
  }
  
  if (isAuthenticated()) {
    server.sendHeader("Location", "/");
    server.send(303);
    return;
  }

  String html = String(PAGE_LOGIN);
  html.replace("%ERROR_MSG%", errorMsg);
  server.send(200, "text/html", html);
}

void WebManager::handleRoot() {
  if (!isAuthenticated()) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }

  String html = String(PAGE_DASHBOARD);
  auto history = dataManager->getHistory();
  auto badges = dataManager->getAuthorizedBadges();

  int maxHistory = 10;
  String histHtml = "";
  int startIdx = max(0, (int)history.size() - maxHistory);
  for (int i = history.size() - 1; i >= startIdx; i--) {
    LogEntry log = history[i];
    histHtml += "<tr><td class='time-calc' data-millis='" + String(log.timestamp) + "'>-</td>";
    histHtml += "<td>" + log.uid + "</td>";
    if (log.success) histHtml += "<td class='succes'>Autorisé</td>";
    else histHtml += "<td class='echec'>Refusé</td>";

    if (dataManager->isAuthorized(log.uid)) 
      histHtml += "<td><a href='/toggle?uid=" + log.uid + "' class='btn btn-del'>Retirer</a></td></tr>";
    else 
      histHtml += "<td><a href='/toggle?uid=" + log.uid + "' class='btn btn-add'>Autoriser</a></td></tr>";
  }
  if(history.size() == 0) histHtml = "<tr><td colspan='4'>Aucun historique pour le moment.</td></tr>";

  String badgeHtml = "";
  for (const auto& b : badges) {
    badgeHtml += "<tr><td>" + b + "</td><td><a href='/toggle?uid=" + b + "' class='btn btn-del'>Révocable</a></td></tr>";
  }
  if(badges.size() == 0) badgeHtml = "<tr><td colspan='2'>Aucun badge autorisé.</td></tr>";

  html.replace("%HISTORY_TABLE%", histHtml);
  html.replace("%BADGE_TABLE%", badgeHtml);
  html.replace("%UPTIME_MS%", String(millis()));
  html.replace("%CURRENT_ACCESS_CODE%", dataManager->getAccessCode());

  server.send(200, "text/html", html);
}

void WebManager::handleToggleBadge() {
  if (!isAuthenticated()) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }
  if (server.hasArg("uid")) {
    String uid = server.arg("uid");
    dataManager->toggleBadge(uid);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void WebManager::handlePassword() {
  if (!isAuthenticated()) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }
  if (server.hasArg("newpwd")) {
    dataManager->updatePassword(server.arg("newpwd"));
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void WebManager::handleAccessCode() {
  if (!isAuthenticated()) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }
  if (server.hasArg("newcode")) {
    String newCode = server.arg("newcode");
    if (newCode.length() == 4 && newCode != "9999" && 
        isDigit(newCode[0]) && isDigit(newCode[1]) && 
        isDigit(newCode[2]) && isDigit(newCode[3])) {
      dataManager->updateAccessCode(newCode);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void WebManager::handleLogout() {
  server.sendHeader("Set-Cookie", "session=deleted; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
  server.sendHeader("Location", "/login");
  server.send(303);
}
