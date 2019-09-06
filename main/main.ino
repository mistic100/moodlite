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

// global brightness
uint8_t brightness;

// turn off everything
boolean isStop = false;

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  // keypad configuration
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

  #ifdef DEBUG
  Serial.println("Keypad ready");
  #endif

  // animations configuration
  animations = new Animations();
  animations->setCurrentMode(RAINBOW);
  
  #ifdef DEBUG
  Serial.println("Animations ready");
  #endif

  // FastLED configuration
  brightness = INITIAL_BRIGHTNESS;

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  #ifdef DEBUG
  Serial.println("FastLED ready");
  #endif

  // screen configuration
  screen = new Screen();
  screen->showLogo();
  
  #ifdef DEBUG
  Serial.println("Setup done");
  #endif

  delay(2000);
}

void loop() {
  EVERY_N_MILLIS_I(timer, 10) {
    timer.setPeriod(animations->period);
    animations->run(isStop);
    FastLED.setBrightness(brightness);
    FastLED.show();
  }

  EVERY_N_MILLIS(500) {
    if (isStop) {
      screen->clear();
    } else {
      screen->show(animations->currentModeName(), brightness);
    }
  }
}

/**
 * TODO
 */
void PressBtnOk() {
  DEBOUNCE(200) {
    #ifdef DEBUG
    Serial.println("Press button OK");
    #endif

    isStop = !isStop;
  }
}

/**
 * Increase brightness
 */
void PressBtnUp() {
  DEBOUNCE(200) {
    #ifdef DEBUG
    Serial.println("Press button UP");
    #endif
    
    brightness = qadd8(brightness, BRIGHTNESS_STEP);
  }
}

/**
 * Decrease brightness
 */
void PressBtnDown() {
  DEBOUNCE(200) {
    #ifdef DEBUG
    Serial.println("Press button DOWN");
    #endif
    
    brightness = qsub8(brightness, BRIGHTNESS_STEP);
  }
}

/**
 * Previous animation
 */
void PressBtnLeft() {
  DEBOUNCE(200) {
    #ifdef DEBUG
    Serial.println("Press button LEFT");
    #endif
    
    animations->setCurrentMode(Modes(animations->currentMode == 0 ? NUM_MODES - 1 : animations->currentMode - 1));
  }
}

/**
 * Next animation
 */
void PressBtnRight() {
  DEBOUNCE(200) {
    #ifdef DEBUG
    Serial.println("Press button RIGHT");
    #endif
    
    animations->setCurrentMode(Modes((animations->currentMode + 1) % NUM_MODES));
  }
}
