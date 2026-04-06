#include "DataManager.h"

DataManager::DataManager() : adminPass("1234"), accessCode("0000"), pendingUnlock(false) {}

void DataManager::begin() {
  prefs.begin("casier", false);
  adminPass = prefs.getString("pwd", "1234");
  accessCode = prefs.getString("accessCode", "0000");
  loadBadges();
}

void DataManager::loadBadges() {
  String data = prefs.getString("badges", "");
  authorizedBadges.clear();
  if (data.length() > 0) {
    int start = 0;
    int end = data.indexOf(',');
    while (end != -1) {
      authorizedBadges.push_back(data.substring(start, end));
      start = end + 1;
      end = data.indexOf(',', start);
    }
    authorizedBadges.push_back(data.substring(start));
  }
}

void DataManager::saveBadges() {
  String data = "";
  for (int i = 0; i < authorizedBadges.size(); i++) {
    data += authorizedBadges[i];
    if (i < authorizedBadges.size() - 1) data += ",";
  }
  prefs.putString("badges", data);
}

void DataManager::addLog(String uid, bool success) {
  if (history.size() >= 50) {
    history.erase(history.begin());
  }
  history.push_back({uid, success, millis()});
}

bool DataManager::isAuthorized(String uid) {
  for (String badge : authorizedBadges) {
    if (badge == uid) return true;
  }
  return false;
}

void DataManager::toggleBadge(String uid) {
  bool found = false;
  for (int i = 0; i < authorizedBadges.size(); i++) {
    if (authorizedBadges[i] == uid) {
      authorizedBadges.erase(authorizedBadges.begin() + i);
      found = true;
      break;
    }
  }
  if (!found) authorizedBadges.push_back(uid);
  saveBadges();
}

void DataManager::updatePassword(String newPwd) {
  adminPass = newPwd;
  prefs.putString("pwd", adminPass);
}

String DataManager::getAdminPass() const {
  return adminPass;
}

String DataManager::getAccessCode() const {
  return accessCode;
}

std::vector<String> DataManager::getAuthorizedBadges() const {
  return authorizedBadges;
}

std::vector<LogEntry> DataManager::getHistory() const {
  return history;
}

void DataManager::updateAccessCode(String code) {
    accessCode = code;
    prefs.putString("accessCode", accessCode);
}

void DataManager::clearHistory() {
    history.clear();
}

void DataManager::setAuthorizedBadges(const std::vector<String>& badges) {
    authorizedBadges = badges;
    saveBadges();
}

bool DataManager::hasPendingRemoteUnlock() const {
    return pendingUnlock;
}

void DataManager::setPendingRemoteUnlock(bool pending) {
    pendingUnlock = pending;
}