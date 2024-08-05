#include <Arduino.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include "credentials.h"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

const int inputPins[] = {16, 17};
const int outputPins[] = {18, 19};
const int analogToDigital = 32;

void initializePinData();

void setPin(const int &pin, const String &type, const int &value) {
  String path = String("/pins/" + String(pin));

  Firebase.setString(firebaseData, String(path + "/type"), type);
  Firebase.setInt(firebaseData, String(path + "/value"), value);
}

// void setPin(int pin, String type, int value) {
//   if (type == "input") {
//     pinMode(pin, INPUT);
//   }

//   if (type == "output") {
//     pinMode(pin, OUTPUT);
//   }

//   if (type == "analog") {
//     analogRead(pin);
//     firebaseData.set
//   }
// }

void parsePin(const String &key, FirebaseJson &json) {
  FirebaseJsonData jsonData;
  json.get(jsonData, key);

  if (jsonData.type == "object") {
    json.get(jsonData, String(key + "/type"));
    String type = jsonData.stringValue;

    json.get(jsonData, String(key + "/value"));
    int value = jsonData.intValue;
  }
}

void streamCallback(StreamData data) {
  Serial.println("Stream data received:");
  Serial.println(data.streamPath());
  Serial.println(data.stringData());

  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json) {
    FirebaseJson &json = data.jsonObject();
    size_t numElements = json.iteratorBegin();
    int type;
    String key, value;

    for (size_t i = 0; i < numElements; i++) {
      json.iteratorGet(i, type, key, value);
      parsePin(key, json);
    }
    json.iteratorEnd();
  }
}

void streamTimeoutCallback(bool timeout) {}

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

  if (!Firebase.beginStream(firebaseData, "/pins")) {
    Serial.printf("Stream begin error, %s\n", firebaseData.errorReason().c_str());
  }

  initializePinData();

  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
}

void loop() { delay(3000); }

void initializePinData() {
  Serial.println("Initialized data");

  for (const int &pin : inputPins) {
    setPin(pin, "input", LOW);
  }

  for (const int &pin : outputPins) {
    setPin(pin, "output", LOW);
  }

  setPin(analogToDigital, "analog", LOW);
}