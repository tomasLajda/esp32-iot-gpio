#include <Arduino.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include "credentials.h"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(300);
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_KEY;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase is connected");
  } else {
    Serial.printf("Firebase failed to connect.");
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {}
