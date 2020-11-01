#include <FastLED.h>
#include <PinChangeInterrupt.h>

#include "constants.h"
#include "debounce.h"
#include "modes.h"
#include "animations.h"
#include "screen.h"

// animation manager
Animations *animations; 

// screen manager
Screen *screen;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BTN_OK, INPUT_PULLUP);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_LEFT, INPUT_PULLUP);
  pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BTN_OK), PressBtnOk, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BTN_UP), PressBtnUp, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BTN_DOWN), PressBtnDown, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BTN_LEFT), PressBtnLeft, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BTN_RIGHT), PressBtnRight, RISING);

  animations = new Animations();
  animations->setMode(RAINBOW);

  screen = new Screen();
  screen->showLogo();

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  delay(1000);
}

void loop() {
  EVERY_N_MILLIS_I(timer, 10) {
    timer.setPeriod(animations->period);
    animations->run();
    FastLED.setBrightness(animations->brightness);
    FastLED.show();
  }

  EVERY_N_MILLIS(500) {
    if (animations->isOff) {
      screen->clear();
    } else {
      screen->show(animations->modeName(), animations->brightness);
    }
  }
}

/**
 * ON/OFF
 */
void PressBtnOk() {
  DEBOUNCE(200) {
    Serial.println("Press button OK");

    if (animations->isOff) {
      animations->on();
    } else {
      animations->off();
    }
  }
}

/**
 * Increase brightness
 */
void PressBtnUp() {
  DEBOUNCE(200) {
    Serial.println("Press button UP");
    
    animations->setBrightness(qadd8(animations->brightness, BRIGHTNESS_STEP));
  }
}

/**
 * Decrease brightness
 */
void PressBtnDown() {
  DEBOUNCE(200) {
    Serial.println("Press button DOWN");
    
    animations->setBrightness(qsub8(animations->brightness, BRIGHTNESS_STEP));
  }
}

/**
 * Previous animation
 */
void PressBtnLeft() {
  DEBOUNCE(200) {
    Serial.println("Press button LEFT");
    
    animations->setMode(Modes(animations->mode == 0 ? NUM_MODES - 1 : animations->mode - 1));
  }
}

/**
 * Next animation
 */
void PressBtnRight() {
  DEBOUNCE(200) {
    Serial.println("Press button RIGHT");
    
    animations->setMode(Modes((animations->mode + 1) % NUM_MODES));
  }
}
