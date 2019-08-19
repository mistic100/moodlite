#pragma once

#include <FastLED.h>

#include "constants.h"
#include "modes.h"

class Animations {

  public:

  // loop period in ms
  int period = 10;
  // palette offset
  int startIndex = 0;
  // current animation
  enum Modes currentMode;
  // current blending mode
  TBlendType currentBlending;
  // displayed palette
  CRGBPalette16 currentPalette;
  // target palette for blending
  CRGBPalette16 targetPalette;
  // leds
  CRGB leds[NUM_LEDS];

  /**
   * Run the current animation
   */
  void run() {
    switch (currentMode) {
      case RAINBOW:
        runRainbow();
        break;
        
      case RAINBOW_2:
        runRainbow2();
        break;
        
      case PARTY:
        runParty();
        break;

      case FIRE:
        runFire();
        break;

      case RANDOM:
        runRandom();
        break;
    }
  }

  /**
   * Change the current animation
   */
  void setCurrentMode(enum Modes mode) {
    currentMode = mode;
    period = 10;
    currentBlending = LINEARBLEND;

    #ifdef DEBUG
    Serial.print("Current mode is ");
    Serial.println(currentModeName());
    #endif
    
    switch (currentMode) {
      case RAINBOW:
        currentPalette = RainbowColors_p;
        break;

      case RAINBOW_2:
        period = 30;
        currentPalette = RainbowColors_p;
        break;
  
      case PARTY:
        currentPalette = PartyColors_p;
        break;
  
      case FIRE:
        period = 30;
        currentPalette = HeatColors_p;
        break;
  
      case RANDOM:
        currentPalette = CRGBPalette16(
          CHSV(random8(), 255, random8(128,255)), 
          CHSV(random8(), 255, random8(128,255))
        );
        targetPalette = CRGBPalette16(
          CHSV(random8(), 255, random8(128,255)), 
          CHSV(random8(), 255, random8(128,255))
        );
        break;
    }
  }

  /**
   * Get the current animation's name
   */
  String currentModeName() {
    switch (currentMode) {
      case RAINBOW: return "Rainbow"; 
      case RAINBOW_2: return "Rainbow 2"; 
      case PARTY: return "Party";
      case FIRE: return "Fire";
      case RANDOM: return "Random";
      default: return "N/A";
    }
  }


  private:

  /**
   * Displays the current palette
   */
  void showPalette(uint8_t range = 255) {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t index = ceil(i/TILE_SIZE) * (range/NUM_TILES);
      leds[i] = ColorFromPalette(currentPalette, index + startIndex, 255, currentBlending);
    }
  }

  /**
   * Run the random animation
   */
  void runRandom() {
    startIndex = 0;
    nblendPaletteTowardPalette(currentPalette, targetPalette, 24);

    EVERY_N_SECONDS(5) {
      targetPalette = CRGBPalette16(
        CHSV(random8(), 255, random8(128,255)), 
        CHSV(random8(), 255, random8(128,255))
      );
    }

    showPalette();
  }

  /**
   * Run the rainbow animation
   */
  void runRainbow() {
    startIndex = startIndex + 1;
    
    showPalette();
  }

  /**
   * Run the rainbow 2 animation
   */
  void runRainbow2() {
    startIndex = startIndex + 1;
    
    showPalette(32);
  }

  /**
   * Run the party animation
   */
  void runParty() {
    startIndex = startIndex + 3;
    
    showPalette();
  }

  /**
   * Run the fire animation
   */
  void runFire() {
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_TILES];

    static uint8_t COOLING = 10;
    static uint8_t SPARKING = 50;
  
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_TILES; i++) {
      heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_TILES) + 2));
    }
    
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_TILES - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 1] + heat[k - 2]) / 4;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING) {
      int y = random8(3);
      heat[y] = qadd8(heat[y], random8(160,255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_TILES; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8(heat[j], 240);
      CRGB color = ColorFromPalette(currentPalette, colorindex);
      
      leds[j * TILE_SIZE] = color;
      leds[j * TILE_SIZE + 1] = color;
      leds[j * TILE_SIZE + 2] = color;
    }
  }

};