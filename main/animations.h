#pragma once

#include <FastLED.h>

#include "constants.h"
#include "modes.h"

class Animations {

  public:
  // leds
  CRGB leds[NUM_LEDS];
  // current animation
  enum Modes mode;
  // current brightness
  uint8_t brightness = INITIAL_BRIGHTNESS;
  // on/off state
  boolean isOff = false;
  // loop period in ms
  int period = 10;
  // displayed palette
  CRGBPalette16 palette;
  // target palette for blending
  CRGBPalette16 targetPalette;
  // palette offset
  int paletteIndex = 0;

  void off() {
    isOff = true;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  void on() {
    isOff = false;
  }

  /**
   * Run the current animation
   */
  void run() {
    if (isOff) {
      return;
    }
    
    switch (mode) {
      case RAINBOW:
        runRainbow();
        break;
        
      case RAINBOW_2:
        runRainbow2();
        break;

      case RAINBOW_3:
        runRainbow3();
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

//      case VUMETER:
//        runVumeter();
//        break;
    }
  }

  /**
   * Change the current animation
   */
  void setMode(enum Modes newMode) {
    mode = newMode;
    period = 10;
    
    switch (mode) {
      case RAINBOW:
        palette = RainbowColors_p;
        break;

      case RAINBOW_2:
        period = 50;
        palette = RainbowColors_p;
        break;

      case RAINBOW_3:
        palette = RainbowColors_p;
        break;
  
      case PARTY:
        palette = PartyColors_p;
        break;
  
      case FIRE:
        period = 30;
        palette = HeatColors_p;
        break;
  
      case RANDOM:
        palette = CRGBPalette16(
          CHSV(random8(), 255, 255), 
          CHSV(random8(), 255, 255)
        );
        targetPalette = CRGBPalette16(
          CHSV(random8(), 255, 255), 
          CHSV(random8(), 255, 255)
        );
        break;
    }

    Serial.print("Current mode is ");
    Serial.println(modeName());
  }

  void setBrightness(uint8_t newBrightness) {
    brightness = newBrightness;
  }

  /**
   * Get the current animation's name
   */
  String modeName() {
    switch (mode) {
      case RAINBOW: return "Rainbow";
      case RAINBOW_2: return "Rainbow 2";
      case RAINBOW_3: return "Rainbow 3";
      case PARTY: return "Party";
      case RANDOM: return "Random";
      case FIRE: return "Fire";
//      case VUMETER: return "Vumeter";
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
      leds[i] = ColorFromPalette(palette, index + paletteIndex);
    }
  }

  /**
   * Run the random animation
   */
  void runRandom() {
    paletteIndex = 0;
    nblendPaletteTowardPalette(palette, targetPalette, 12);

    EVERY_N_SECONDS(5) {
      targetPalette = CRGBPalette16(
        CHSV(random8(), 255, 255), 
        CHSV(random8(), 255, 255)
      );
    }

    showPalette();
  }

  /**
   * Run the rainbow animation
   */
  void runRainbow() {
    paletteIndex--;
    
    showPalette();
  }

  /**
   * Run the rainbow 2 animation
   */
  void runRainbow2() {
    paletteIndex--;
    
    showPalette(32);
  }

  /**
   * Run the rainbow 3 animation
   */
  void runRainbow3() {
    paletteIndex--;

    static int index2 = 0;
    EVERY_N_MILLIS(60) {
      index2 = index2 + 1;
    }
    
    showPalette(128);

    for (int i = 0; i < NUM_LEDS; i++) {
      if ((i + index2) % 11 < 5) {
        leds[i] = CRGB::Black;
      }
    }
  }

  /**
   * Run the party animation
   */
  void runParty() {
    paletteIndex-= 3;
    
    showPalette();
  }

  /**
   * Run the vumeter animation
   */
//  void runVumeter() {
//    static uint16_t previous = 0;
//
//    uint16_t vol = analogRead(1);
//
//    // slow decay & fast attack
//    if (vol > previous) {
//      int newVol = previous + 128;
//      previous = min(1023, newVol);
//    } else {
//      int newVol = previous - 32;
//      previous = max(0, newVol);
//    }
//    
//    uint8_t mid = previous / 1024.f * NUM_TILES;
//
//    if (mid != 0) {
//      fill_gradient_RGB(leds, 0, CRGB::Blue, mid * TILE_SIZE, CRGB::DodgerBlue);
//    }
//
//    if (mid != NUM_TILES) {
//      fill_gradient_RGB(leds, mid * TILE_SIZE + TILE_SIZE - 1, CRGB::Crimson, NUM_LEDS - 1, CRGB::Red);
//    }
//
//    fill_gradient_RGB(leds, mid * TILE_SIZE, CRGB::GhostWhite, mid * TILE_SIZE + TILE_SIZE - 1, CRGB::GhostWhite);
//  }

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
      CRGB color = ColorFromPalette(palette, colorindex);
      
      leds[j * TILE_SIZE] = color;
      leds[j * TILE_SIZE + 1] = color;
      leds[j * TILE_SIZE + 2] = color;
    }
  }

};
