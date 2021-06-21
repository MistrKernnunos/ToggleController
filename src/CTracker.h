//
// Created by machavi on 6/16/21.
//
#ifndef CTracker_h
#define CTracker_h
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <list>
#include <string>

class CTracker {
public:
  CTracker(const String &apiKey, const String &name, const int pid,
           std::list<String> tags = {})
      : name(name), apiKey(apiKey), pid(pid), tags(tags){};

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
  static char *sha1;
  static char *apiToken;
  std::list<String> tags;
};

char *CTracker::sha1 =
    "6E:FB:32:84:1D:FA:F2:9E:78:25:52:93:AF:5B:21:29:48:71:91:2C";

bool CTracker::Start() {
  if (WiFi.status() == WL_CONNECTED) {
    char jsonOutput[256];

    HTTPClient http;
    WiFiClientSecure client;
    http.useHTTP10();
    client.setFingerprint(sha1);
    http.begin(client, "https://api.track.toggl.com/api/v8/time_entries/start");
    http.setAuthorization(apiKey.c_str(), "api_token");

    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    JsonObject obj = doc.createNestedObject("time_entry");
    obj["description"] = name;
    obj["pid"] = pid;

    if (!empty(tags)) {
      JsonArray tagsArray = obj.createNestedArray("tags");
      for (auto elem : tags) {
        tagsArray.add(elem);
      }
    }

    obj["created_with"] = "tlacitko";
    serializeJson(doc, jsonOutput);
    Serial.println(String(jsonOutput));
    int httpCode = http.POST(String(jsonOutput));

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      StaticJsonDocument<512> returnDoc;
      deserializeJson(returnDoc, http.getStream());
      JsonObject retData = returnDoc["data"];
      id = retData["id"];
      running = true;
      Serial.println(id);

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
  client.setFingerprint(sha1);
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
    client.setFingerprint(sha1);
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