# Code Arduino / ESP32 pour Casiers Connectés

Voici un exemple complet pour votre ESP32 (utilisant l'IDE Arduino) qui explique comment il doit interagir avec l'API que nous venons de créer.

Ce code va :
1. Se connecter à votre réseau WiFi.
2. Lire l'UID d'un badge scanné (simulation ici, à remplacer par votre code RFID RC522 ou autre).
3. Envoyer une requête HTTP POST à l'API `http://VOTRE_IP:3000/api/access`.
4. Analyser la réponse JSON pour autoriser ou refuser l'ouverture du casier.

## Dépendances requises dans l'IDE Arduino
*   Bibliothèque WiFi (incluse avec l'ESP32)
*   Bibliothèque **ArduinoJson** (à installer via le gestionnaire de bibliothèques)
*   Bibliothèque HTTPClient (incluse avec l'ESP32)

---

## Code d'exemple (`esp32_locker.ino`)

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Remplacez par vos identifiants WiFi
const char* ssid = "VOTRE_SSID_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE_WIFI";

// Remplacez par l'adresse IP de l'ordinateur qui exécute le backend (Node.js)
const char* serverUrl = "http://192.168.1.XX:3000/api/access"; 

// L'ID du casier que cet ESP32 contrôle spécifique
const int lockerId = 1; 

// Pin relai pour ouvrir le casier
const int RELAY_PIN = 23; 

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Casier fermé par défaut

  // Connexion au WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion au WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au réseau WiFi!");
}

void loop() {
  // --- ÉTAPE 1 : Attendre qu'un badge soit scanné ---
  // (Ici vous mettrez votre code pour lire le lecteur RFID)
  // Simulation d'un scan de badge toutes les 10 secondes :
  
  if (WiFi.status() == WL_CONNECTED) {
    // Par exemple, on a lu le badge "A1B2C3D4" (Badge Admin)
    String scannedBadgeUid = "A1B2C3D4"; 
    
    Serial.println("Badge scanné : " + scannedBadgeUid);
    verifierAcces(scannedBadgeUid, lockerId);

    // On attend 10 secondes avant la prochaine simulation
    delay(10000); 
  } else {
    Serial.println("Erreur : WiFi déconnecté.");
    delay(5000); // Tenter de se reconnecter plus tard...
  }
}

void verifierAcces(String uid, int idCasier) {
  HTTPClient http;
  
  // Prépare la requête HTTP POST
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // Crée le JSON à envoyer : {"badge_uid": "A1B2C3D4", "locker_id": 1}
  StaticJsonDocument<200> doc;
  doc["badge_uid"] = uid;
  doc["locker_id"] = idCasier;
  
  String requestBody;
  serializeJson(doc, requestBody);

  Serial.println("Envoi requête API : " + requestBody);
  
  // Envoi de la requête POST
  int httpResponseCode = http.POST(requestBody);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Réponse API : " + response);

    // Analyse de la réponse JSON {"granted": true, "message": "Access granted"}
    StaticJsonDocument<200> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);

    if (!error) {
      bool accessGranted = responseDoc["granted"];
      
      if (accessGranted) {
        Serial.println(">>> ACCÈS AUTORISÉ ! Ouverture du casier...");
        ouvrirCasier();
      } else {
        Serial.println("<<< ACCÈS REFUSÉ !");
      }
    } else {
      Serial.println("Erreur lors de la lecture du JSON serveur.");
    }
  } else {
    Serial.print("Erreur de requête HTTP. Code: ");
    Serial.println(httpResponseCode);
  }
  
  // Libère les ressources
  http.end(); 
}

void ouvrirCasier() {
  // Active le relais pendant 3 secondes
  digitalWrite(RELAY_PIN, HIGH);
  delay(3000);
  digitalWrite(RELAY_PIN, LOW); // Referme le relais
  Serial.println("Casier refermé (relais désactivé).");
}
```

## Instructions pour tester :

1.  Lancez le backend Node (`node index.js` dans le dossier backend).
2.  Assurez-vous que l'ordinateur (backend) et l'ESP32 sont sur le **même réseau WiFi**.
3.  Modifiez la variable `serverUrl` dans le code Arduino avec l'**adresse IP locale (IPv4)** de votre ordinateur.
4.  Modifiez le SSID et le Mot de passe de votre WiFi.
5.  Uploadez sur l'ESP32 et ouvrez le moniteur série ! L'historique des requêtes s'affichera en temps réel sur le site web.
