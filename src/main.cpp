#include "CController.h"
#include "CTracker.h"
#include <Arduino.h>
#include <Bounce2.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

const char *ssid = "The best phone ever";
const char *pass = "nejlepsitelefon";
const String apiKey = "b6cc0cd9766d2a831d906102722fa6fa";

CController controller;

bool ledState = false;

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  controller.AddTracker(apiKey, "test", 161879008, A0, {"test", "zabava"});

  controller.Start();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);
}

void loop() {
  controller.Update();

  //  button.update();
  //  timer.update();
  //  if (button.pressed()) {
  //    Serial.println("pressed");
  //    if (tracker.running) {
  //      tracker.Stop();
  //    } else {
  //      tracker.Start();
  //    }
  //  }
  if (controller.running) {
    ledState = false;
  } else {
    ledState = true;
  }
  digitalWrite(LED_BUILTIN, ledState);
}
