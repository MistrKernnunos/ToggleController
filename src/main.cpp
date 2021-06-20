#include "CController.h"
#include "CTracker.h"
#include "Constanst.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#define LED_COUNT 4

CController controller(10000, D2, 4);

//bool ledState = false;

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

  controller.AddTracker(apiKey, "tes1", 161879008, D1, {"test", "zabava"});
  controller.AddTracker(apiKey, "test2", 161879008, D3, {"test", "zabava"});
  controller.AddTracker(apiKey, "test3", 161879008, D4, {"test", "zabava"});

  controller.Start();

//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, ledState);
}

void loop() {
  controller.Update();

//  if (controller.running) {
//    ledState = false;
//  } else {
//    ledState = true;
//  }
//  digitalWrite(LED_BUILTIN, ledState);
}
