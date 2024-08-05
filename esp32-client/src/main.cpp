#include <Arduino.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include "credentials.h"

struct Pin {
  int id;
  String type;
  byte value;
};

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

Pin pins[] = {{16, "input", LOW},
              {17, "input", LOW},
              {18, "output", LOW},
              {19, "output", LOW},
              {32, "analog", LOW}};

void initializePinData();

void setPin(Pin &newPin) {
  for (auto &pin : pins) {
    if (pin.id == newPin.id) {
      pin = newPin;
    }
  }
}

void readPin(Pin &pin) {
  if (pin.type == "output") {
    int value = digitalRead(pin.id);

    if (value != pin.value) {
      pin.value = value;
      updatePinDb(pin);
    }
  } else if (pin.type == "analog") {
    int value = analogRead(pin.id);

    const int threshold = 10;
    if (abs(value - pin.value) > threshold) {
      pin.value = value;
      updatePinDb(pin);
    }
  }
}

void updatePinDb(const Pin &pin) {
  String path = String("/pins/" + String(pin.id));

  Firebase.setString(firebaseData, String(path + "/type"), pin.type);
  Firebase.setInt(firebaseData, String(path + "/value"), pin.value);
}

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

  for (const auto &pin : pins) {
    updatePinDb(pin);
  }
}