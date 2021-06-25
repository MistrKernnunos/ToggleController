//
// Created by machavi on 6/20/21.
//

#ifndef TOGGLECONTROLLER_CWEBSERVER_H
#define TOGGLECONTROLLER_CWEBSERVER_H

#include "ArduinoJson.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClientSecure.h>
#include <vector>

class CWebServer {
public:
  CWebServer(String apiKey, int port = 80) : server(port), apiKey(apiKey){};

  bool StartServer();
  void handleClient();

private:
  // functions for handling connections
  void handleNotFound();
  void handleOnConnect();
  void handleDataRequest();
  void handleConfig();
  std::vector<int> getWorkspaces();
  std::vector<String> getTags(const std::vector<int> &workspaces);
  std::vector<std::pair<String, int>>
  getProjects(const std::vector<int> &workspaces);
  static String sha1;
  ESP8266WebServer server;
  String apiKey;
  std::vector<int> m_Projects;
};
String CWebServer::sha1 =
    "6E:FB:32:84:1D:FA:F2:9E:78:25:52:93:AF:5B:21:29:48:71:91:2C";
bool CWebServer::StartServer() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", [this]() { this->handleOnConnect(); });
    server.onNotFound([this]() { this->handleNotFound(); });
    server.on("/data", [this]() { this->handleDataRequest(); });
    server.on("/config", [this]() { this->handleConfig(); });

    server.begin();
    Serial.println("HTTP server running");
    if (!LittleFS.begin()) {
      Serial.println("littlefs problem");
    }
    return true;
  }
  return false;
}
void CWebServer::handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}
void CWebServer::handleOnConnect() {
  auto html = LittleFS.open("ConfigPage.html", "r");
  //  Serial.println(html.readString());
  server.send(200, "text/html", html.readString());
}
void CWebServer::handleClient() { server.handleClient(); }

void CWebServer::handleDataRequest() {
  auto workspaces = getWorkspaces();
  auto tags = getTags(workspaces);
  auto projects = getProjects(workspaces);
  DynamicJsonDocument doc(1024);
  JsonObject tagsObj = doc.createNestedObject("tags");

  JsonArray tagsArray = tagsObj.createNestedArray("results");
  for (size_t i = 0; i < tags.size(); ++i) {
    JsonObject object = tagsArray.createNestedObject();
    object["id"] = i;
    object["text"] = tags[i];
  }

  JsonObject projectsObj = doc.createNestedObject("projects");

  JsonArray projectsArr = projectsObj.createNestedArray("results");
  for (size_t i = 0; i < projects.size(); ++i) {
    JsonObject object = projectsArr.createNestedObject();
    object["id"] = projects[i].second;
    object["text"] = projects[i].first;
  }
  String json;
  serializeJsonPretty(doc, json);
#ifdef DEBUG_Data
  Serial.println(json);
#endif
  server.send(200, "application/json", json);
}
std::vector<int> CWebServer::getWorkspaces() {
  String url = "https://api.track.toggl.com/api/v8/workspaces";
  HTTPClient http;
  WiFiClientSecure client;
  http.useHTTP10();
  client.setFingerprint(sha1.c_str());
  http.begin(client, url);
  http.setAuthorization(apiKey.c_str(), "api_token");

  http.addHeader("Content-Type", "application/json");

  std::vector<int> workspaces;

  int httpCode = http.GET();

  if (httpCode > 0) {

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, http.getStream());
    http.end();
    size_t num = doc.size();

#ifdef DEBUG_WebServer
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    Serial.println(num);
    String json;
    serializeJsonPretty(doc, json);
    Serial.println(json);
#endif
    for (size_t i = 0; i < doc.size(); ++i) {
      if (doc[i]["id"] != nullptr) {
        workspaces.push_back(doc[i]["id"]);
        Serial.println((int)doc[i]["id"]);
      }
    }
  }
  return workspaces;
}
std::vector<String> CWebServer::getTags(const std::vector<int> &workspaces) {
  std::vector<String> tags;
  String url = "https://api.track.toggl.com/api/v8/workspaces/";
  HTTPClient http;
  WiFiClientSecure client;
  http.useHTTP10();
  client.setFingerprint(sha1.c_str());

  for (auto elem : workspaces) {
    String completeUrl(url);
    completeUrl += String(elem) += "/tags";
    http.begin(client, completeUrl);
    http.setAuthorization(apiKey.c_str(), "api_token");

    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();
    if (httpCode > 0) {

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getStream());
      http.end();

#ifdef DEBUG_WebServer
      Serial.println(completeUrl);
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      String json;
      serializeJsonPretty(doc, json);
      Serial.println(json);
#endif

      for (size_t i = 0; i < doc.size(); ++i) {
        if (doc[i]["name"] != nullptr) {
          tags.push_back(doc[i]["name"]);
#ifdef DEBUG_WebServer
          Serial.println(tags.back());
#endif
        }
      }
    }
  }
  return tags;
}
std::vector<std::pair<String, int>>
CWebServer::getProjects(const std::vector<int> &workspaces) {
  std::vector<std::pair<String, int>> projects;
  String url = "https://api.track.toggl.com/api/v8/workspaces/";
  HTTPClient http;
  WiFiClientSecure client;
  http.useHTTP10();
  client.setFingerprint(sha1.c_str());

  for (auto elem : workspaces) {
    String completeUrl(url);
    completeUrl += String(elem) += "/projects";
    http.begin(client, completeUrl);
    http.setAuthorization(apiKey.c_str(), "api_token");

    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();
    if (httpCode > 0) {

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getStream());
      http.end();

#ifdef DEBUG_WebServer
      Serial.println(completeUrl);
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      String json;
      serializeJsonPretty(doc, json);
      Serial.println(json);
#endif

      for (size_t i = 0; i < doc.size(); ++i) {
        if (doc[i]["id"] != nullptr) {
          projects.push_back(std::make_pair(doc[i]["name"], doc[i]["id"]));
#ifdef DEBUG_WebServer
          Serial.println(projects.back().first);
#endif
        }
      }
    }
  }
  return projects;
}
void CWebServer::handleConfig() {
  if (server.hasArg("plain") == false) {
    server.send(200, "test/plain", "Body not received");
    return;
  }
  String payload = server.arg("plain");
  LittleFS.begin();
  auto file = LittleFS.open("./config.json", "w");
  file.write(payload.c_str());
  file.close();
  LittleFS.end();

  server.send(200, "test/plain", "Configuration saved");
}

#endif // TOGGLECONTROLLER_CWEBSERVER_H
