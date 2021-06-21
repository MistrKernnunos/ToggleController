//
// Created by machavi on 6/20/21.
//

#ifndef TOGGLECONTROLLER_CWEBSERVER_H
#define TOGGLECONTROLLER_CWEBSERVER_H

#include "ArduinoJson.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

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
  JsonArray getProjects();

  String apiKey;
  ESP8266WebServer server;
};
bool CWebServer::StartServer() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", [this]() { this->handleOnConnect(); });
    server.onNotFound([this]() { this->handleNotFound(); });

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

void CWebServer::handleDataRequest() {}
JsonArray CWebServer::getProjects() {}

#endif // TOGGLECONTROLLER_CWEBSERVER_H
