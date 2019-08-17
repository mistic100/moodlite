#include <FastLED.h>
#include <PinChangeInterrupt.h>

#include "constants.h"
#include "modes.h"
#include "animations.h"

// animation manager
Animations *animations; 

// global brightness
uint8_t brightness;

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  delay(1000);

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
  brightness = 64;

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  
  #ifdef DEBUG
  Serial.println("FastLED ready");
  #endif
  
  #ifdef DEBUG
  Serial.println("Setup done");
  #endif
}

void loop() {
  EVERY_N_MILLIS_I(timer, 10) {
    timer.setPeriod(animations->period);
    animations->run();
    FastLED.show();
  }
}

/**
 * TODO
 */
void PressBtnOk() {
  #ifdef DEBUG
  Serial.println("Press button OK");
  #endif
  
}

/**
 * Increase brightness
 */
void PressBtnUp() {
  #ifdef DEBUG
  Serial.println("Press button UP");
  #endif
  
  brightness = qadd8(brightness, BRIGHTNESS_STEP);
  FastLED.setBrightness(brightness);
}

/**
 * Decrease brightness
 */
void PressBtnDown() {
  #ifdef DEBUG
  Serial.println("Press button DOWN");
  #endif
  
  brightness = qsub8(brightness, BRIGHTNESS_STEP);
  FastLED.setBrightness(brightness);
}

/**
 * Previous animation
 */
void PressBtnLeft() {
  #ifdef DEBUG
  Serial.println("Press button LEFT");
  #endif
  
  animations->setCurrentMode(Modes(animations->currentMode == 0 ? NUM_MODES - 1 : animations->currentMode - 1));
}

/**
 * Next animation
 */
void PressBtnRight() {
  #ifdef DEBUG
  Serial.println("Press button RIGHT");
  #endif
  
  animations->setCurrentMode(Modes((animations->currentMode + 1) % NUM_MODES));
}
