#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <Preferences.h>
#include <vector>
#include <Arduino.h>

struct LogEntry {
  String uid;
  bool success;
  unsigned long timestamp;
};

class DataManager {
private:
  Preferences prefs;
  std::vector<String> authorizedBadges;
  std::vector<LogEntry> history;
  String adminPass;
  String accessCode;
  
public:
  DataManager();
  
  void begin();
  void loadBadges();
  void saveBadges();
  void addLog(String uid, bool success);
  bool isAuthorized(String uid);
  void toggleBadge(String uid);
  void updatePassword(String newPwd);
  void updateAccessCode(String newCode);
  
  String getAdminPass() const;
  String getAccessCode() const;
  std::vector<String> getAuthorizedBadges() const;
  std::vector<LogEntry> getHistory() const;
};

#endif
