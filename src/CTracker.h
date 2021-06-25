//
// Created by machavi on 6/16/21.
//
#ifndef CTracker_h
#define CTracker_h
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <list>
#include <string>

class CTracker {
public:
  CTracker(const String &apiKey, const String &name, const int pid,
           size_t jsonIndex, std::list<String> tags = {})
      : name(name), apiKey(apiKey), pid(pid), jsonIndex(jsonIndex),
        tags(tags){};

  bool Start();
  bool Stop();
  bool Stopped();
  bool Update();

  volatile bool running = false;

  String name;

private:
  String apiKey;
  int pid;
  int id = 0;
  static String sha1;
  size_t jsonIndex;
  std::list<String> tags;
};

String CTracker::sha1 =
    "6E:FB:32:84:1D:FA:F2:9E:78:25:52:93:AF:5B:21:29:48:71:91:2C";

bool CTracker::Start() {
  if (WiFi.status() == WL_CONNECTED) {
    char jsonOutput[256];

    HTTPClient http;
    WiFiClientSecure client;
    http.useHTTP10();
    client.setFingerprint(sha1.c_str());
    http.begin(client, "https://api.track.toggl.com/api/v8/time_entries/start");
    http.setAuthorization(apiKey.c_str(), "api_token");

    http.addHeader("Content-Type", "application/json");

    LittleFS.begin();
    auto file = LittleFS.open("./config.json", "r");

    DynamicJsonDocument doc(1024);
    DynamicJsonDocument payload(1024);
    deserializeJson(doc, file.readString());
<<<<<<< HEAD
    LittleFS.end();
=======
>>>>>>> 90df00c3e52369076f2e69a3e9d0c4c71dc39769

#ifdef DEBUG_Tracker
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    String test;
    serializeJsonPretty(doc, test);
    Serial.println(test);
#endif

    if (doc.as<JsonArray>()[jsonIndex] == nullptr) {
      // TODO show on display
      Serial.println("no config");
      return false;
    }

    JsonObject obj = payload.createNestedObject("time_entry");
    obj.set(doc.as<JsonArray>()[jsonIndex].as<JsonObject>());

    obj["created_with"] = "tlacitko";
    serializeJson(payload, jsonOutput);

    Serial.println(String(jsonOutput));

    int httpCode = http.POST(String(jsonOutput));
    LittleFS.end();

    if (httpCode > 0) {
      StaticJsonDocument<512> returnDoc;
      deserializeJson(returnDoc, http.getStream());
      JsonObject retData = returnDoc["data"];
      id = retData["id"];
      running = true;

#ifdef DEBUG_Tracker
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      Serial.println(id);
#endif
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
      String payload = http.getString();
      Serial.println(payload);
    }

    http.end();
    return true;
  }
  return false;
}
bool CTracker::Stop() {

  HTTPClient http;
  WiFiClientSecure client;
  http.useHTTP10();
  client.setFingerprint(sha1.c_str());
  // prepare url
  String stopUrl = "https://api.track.toggl.com/api/v8/time_entries/";
  stopUrl += String(id) += "/stop";

  http.begin(client, stopUrl);
  http.setAuthorization(apiKey.c_str(), "api_token");

  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT("{}");

  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    running = false;
    Serial.println("stopped by user");

  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
  }

  http.end();
  return true;
}
bool CTracker::Update() {
  if (running) {

    String currUrl = "https://api.track.toggl.com/api/v8/time_entries/current";

    HTTPClient http;
    WiFiClientSecure client;
    http.useHTTP10();
    client.setFingerprint(sha1.c_str());
    http.begin(client, currUrl);
    http.setAuthorization(apiKey.c_str(), "api_token");

    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      StaticJsonDocument<512> returnDoc;
      deserializeJson(returnDoc, http.getStream());

      JsonObject retData = returnDoc["data"];
      if (retData["id"] != id) {
        running = false;
        Serial.println("stopped from web ");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }

    http.end();
    return true;
  }
  return false;
}
bool CTracker::Stopped() {
  running = false;
  return false;
}
#endif // CTracker_h