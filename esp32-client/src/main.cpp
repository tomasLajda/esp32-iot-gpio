#include <Arduino.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include "credentials.h"

struct Pin {
  byte id;
  String type;
  int value;

  bool operator==(const Pin &pin) const {
    return id == pin.id && type == pin.type && value == pin.value;
  }

  bool operator!=(const Pin &pin) const { return !(*this == pin); }
};

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

Pin pins[] = {{16, "input", LOW},
              {17, "input", LOW},
              {18, "output", LOW},
              {19, "output", LOW},
              {32, "analog", LOW}};

void setPin(Pin &oldPin, Pin &currentPin) {
  if (oldPin == currentPin) return;

  if (currentPin.type == "output") {
    pinMode(currentPin.id, OUTPUT);
  }

  if (currentPin.type == "input" || currentPin.type == "analog") {
    pinMode(oldPin.id, INPUT);
  }

  oldPin = currentPin;
}

void updatePinDb(const Pin &pin) {
  if (Firebase.ready()) {
    String path = String("/pins/" + String(pin.id));

    if (!Firebase.setInt(firebaseData, String(path + "/value"), pin.value)) {
      Serial.println("Failed to send value: " + firebaseData.errorReason());
    }
  }

  delay(100);
}

void initializePinData() {
  Serial.println("Initialized data");

  for (const auto &pin : pins) {
    updatePinDb(pin);
  }
}

void readPin(Pin &pin) {
  if (pin.type == "input") {
    int value = digitalRead(pin.id);

    if (value != pin.value) {
      pin.value = value;
      updatePinDb(pin);
    }
  } else if (pin.type == "analog") {
    int value = analogRead(pin.id);

    const int threshold = 50;
    if (abs(value - pin.value) > threshold) {
      pin.value = value;
      updatePinDb(pin);
    }
  }
}

void parsePin(const String &key, FirebaseJson &json) {
  FirebaseJsonData jsonData;
  json.get(jsonData, key);

  if (jsonData.type == "object") {
    json.get(jsonData, String(key + "/type"));
    String type = jsonData.stringValue;

    json.get(jsonData, String(key + "/value"));
    int value = jsonData.intValue;

    byte id = (byte)key.toInt();
    if (id != 0 || key == "0") {
      Pin pin = {id, type, value};

      for (auto &oldPin : pins) {
        if (oldPin.id == pin.id) {
          setPin(oldPin, pin);
          break;
        }
      }
    }
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

void loop() {
  for (auto &pin : pins) {
    if (pin.type == "analog" || pin.type == "input") {
      readPin(pin);
    }
    if (pin.type == "output") {
      digitalWrite(pin.id, pin.value);
    }
  }

  delay(1000);
}
