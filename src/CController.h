//
// Created by machavi on 6/17/21.
//

#ifndef TOGGLECONTROLLER_CCONTROLLER_H
#define TOGGLECONTROLLER_CCONTROLLER_H

#include "CLeds.h"
#include "CTracker.h"
#include <Bounce2.h>
#include <Ticker.h>

class CController {
public:
  CController(int updateInterval = 10000, int ledPin = D2, int ledCount = 3)
      : timer([this]() { this->update(); }, updateInterval),
        leds(ledPin, ledCount) {}

  void AddTracker(const String &apiKey, const String &name, const int pid,
                  int pin, std::list<String> tags);
  bool Start();
  void Update();
  bool running = false;

private:
  void update();
  size_t runningIndex = 0;
  std::vector<CTracker> trackers;
  std::vector<Bounce2::Button> buttons;
  Ticker timer;
  CLeds leds;
};
void CController::AddTracker(const String &apiKey, const String &name,
                             const int pid, int pin,
                             std::list<String> tags = {}) {
  size_t index = trackers.size();
  trackers.emplace_back(apiKey, name, pid, index, tags);
  buttons.push_back(Bounce2::Button());
  buttons.back().attach(pin, INPUT_PULLUP);
  buttons.back().interval(5);
  buttons.back().setPressedState(LOW);
}
bool CController::Start() {

  timer.start();
  return true;
}
void CController::Update() {
  leds.Update();
  timer.update();
  for (size_t i = 0; i < buttons.size(); ++i) {
    buttons[i].update();
    if (buttons[i].pressed()) {
      Serial.println("pressed");
      Serial.println(trackers[i].name);
      // start different timer that the current running
      if (running && runningIndex != i) {
        trackers[runningIndex].Stopped();
        runningIndex = i;
        trackers[runningIndex].Start();
        //        leds.Ripple(1, 255, 0, 0, leftToRight, 0.95);
      } else if (running && runningIndex == i) {
        trackers[runningIndex].Stop();
        leds.Stop();
        running = false;
        //        leds.Stop();
      } else {
        trackers[i].Start();
        runningIndex = i;
        running = true;
        leds.Terminator(10, 255, 0, 0, 0.75, 100);
        //        leds.Ripple(1, 255, 0, 0, leftToRight, 0.95);
      }
    }
  }
}
void CController::update() {
  if (running) {
    trackers[runningIndex].Update();
    running = trackers[runningIndex].running;
    if (!running) {
      leds.Stop();
    }
  }
}

#endif // TOGGLECONTROLLER_CCONTROLLER_H
