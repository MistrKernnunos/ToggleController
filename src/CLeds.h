//
// Created by machavi on 6/18/21.
//

#ifndef TOGGLECONTROLLER_CLEDS_H
#define TOGGLECONTROLLER_CLEDS_H

#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <array>

enum effects { RIPPLE, PULSE, NON, TERMINATOR };
enum direction { leftToRight, rightToLeft };

class CLeds {
public:
  CLeds(int pin, size_t count, int updateInerval = 20)
      : timer([this]() { this->update(); }, updateInerval),
        leds(count, pin, NEO_GRB + NEO_KHZ800), count(count) {
    leds.begin();
    leds.clear();
    leds.show();
    ledColors = new uint8_t *[count];
    for (size_t i = 0; i < count; ++i) {
      ledColors[i] = new uint8_t[3];
      for (int j = 0; j < 3; ++j) {
        ledColors[i][j] = 0;
      }
    }
  };
  void Update();
  void Stop();
  //  void Pulse(int speed, uint8_t R, uint8_t G, uint8_t B);
  void Ripple(int Speed, uint8_t R, uint8_t G, uint8_t B, direction direct,
              float decay, uint32_t duration);
  void Terminator(int Speed, uint8_t R, uint8_t G, uint8_t B, float decay,
                  uint32_t duration);

private:
  void update();
  //  void updatePulse();
  void updateRipple();
  void updateTerminator();
  void pushToLeds();

  Ticker timer;
  Adafruit_NeoPixel leds;
  effects currEffect;

  uint8_t **ledColors;

  size_t count;
  int speed = 1;
  bool running = false;
  uint8_t currPosition = 0;
  direction dir = rightToLeft;
  uint8_t color[3];
  float decayRatio = 0.95;
};
void CLeds::update() {
  switch (currEffect) {
  case NON:
    return;
    break;
  case PULSE:
    break;
  case RIPPLE:
    updateRipple();
    break;
  case TERMINATOR:
    updateTerminator();
    break;
  }
  pushToLeds();
}
void CLeds::Stop() {
  timer.stop();
  running = false;

  for (size_t i = 0; i < count; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      ledColors[i][j] = 0;
    }
  }
  leds.clear();
  leds.show();
}
void CLeds::Ripple(int Speed, uint8_t R, uint8_t G, uint8_t B, direction direct,
                   float decay, uint32_t duration) {
  timer.interval(duration / leds.numPixels());
  timer.start();
  color[0] = R;
  color[1] = G;
  color[2] = B;
  speed = Speed;
  dir = direct;
  running = true;
  decayRatio = decay;
  currEffect = RIPPLE;
  if (dir == leftToRight) {
    currPosition = count - 1;
  } else {
    currPosition = 0;
  }
}
void CLeds::updateRipple() {
  if (running) {
    if (dir == leftToRight) {
      if (currPosition == 0) {
        currPosition = count;
      }
      currPosition--;
    } else {
      currPosition++;
      if (currPosition == count) {
        currPosition = 0;
      }
    }
    ledColors[currPosition][0] = color[0];
    ledColors[currPosition][1] = color[1];
    ledColors[currPosition][2] = color[2];

    for (size_t i = 0; i < count; ++i) {
      for (size_t j = 0; j < 3; ++j) {
        ledColors[i][j] *= decayRatio;
      }
    }
  }
}
void CLeds::pushToLeds() {
  for (size_t i = 0; i < count; ++i) {
    leds.setPixelColor(i, ledColors[i][0], ledColors[i][1], ledColors[i][2]);
#ifdef DEBUG
    Serial.printf("led: %d, red %d, green %d, blue%d\n", i, ledColors[i][0],
                  ledColors[i][1], ledColors[i][2]);
#endif
  }
  leds.show();
}
void CLeds::Update() { timer.update(); }
void CLeds::Terminator(int Speed, uint8_t R, uint8_t G, uint8_t B, float decay,
                       uint32_t duration) {

  timer.interval(duration / leds.numPixels());
  timer.start();
  color[0] = R;
  color[1] = G;
  color[2] = B;
  speed = Speed;
  dir = leftToRight;
  running = true;
  decayRatio = decay;
  currEffect = TERMINATOR;
  currPosition = 1;
}
void CLeds::updateTerminator() {
  if (running) {
    if (dir == leftToRight) {
      currPosition--;
      if (currPosition == 0) {
        dir = rightToLeft;
      }
    } else {
      currPosition++;
      if (currPosition == count - 1) {
        dir = leftToRight;
      }
    }
    ledColors[currPosition][0] = color[0];
    ledColors[currPosition][1] = color[1];
    ledColors[currPosition][2] = color[2];

#ifdef DEBUG
    Serial.printf("led %d, R %d, G %d, B %d\n", currPosition, color[0],
                  color[1], color[2]);
    Serial.printf("led: %d, red %d, green %d, blue%d\n", currPosition,
                  ledColors[currPosition][0], ledColors[currPosition][1],
                  ledColors[currPosition][2]);
#endif

    for (size_t i = 0; i < count; ++i) {
      for (size_t j = 0; j < 3; ++j) {
        ledColors[i][j] *= decayRatio;
      }
    }
  }
}

#endif // TOGGLECONTROLLER_CLEDS_H
