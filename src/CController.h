//
// Created by machavi on 6/17/21.
//

#ifndef TOGGLECONTROLLER_CCONTROLLER_H
#define TOGGLECONTROLLER_CCONTROLLER_H

#include "CTracker.h"
#include <Bounce2.h>
#include <Ticker.h>

class CController {
public:
  CController(int updateInterval = 10000)
      : timer([this]() { this->update(); }, updateInterval) {}

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
};
void CController::AddTracker(const String &apiKey, const String &name,
                             const int pid, int pin,
                             std::list<String> tags = {}) {
  trackers.emplace_back(apiKey, name, pid, tags);
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
  timer.update();
  for (size_t i = 0; i < buttons.size(); ++i) {
    buttons[i].update();
    if (buttons[i].pressed()) {
      Serial.println("pressed");
      // start different timer that the current running
      if (running && runningIndex != i) {
        trackers[runningIndex].Stopped();
        runningIndex = i;
        trackers[runningIndex].Start();
      } else if (running && runningIndex == i) {
        trackers[runningIndex].Stop();
        running = false;
      } else {
        trackers[i].Start();
        runningIndex = i;
        running = true;
      }
    }
  }
}
void CController::update() {
  if (running) {
    trackers[runningIndex].Update();
    running = trackers[runningIndex].running;
  }
}

#endif // TOGGLECONTROLLER_CCONTROLLER_H
