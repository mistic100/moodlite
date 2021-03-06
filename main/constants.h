#pragma once

#define PIN_LED 13
#define PIN_BTN_OK 2
#define PIN_BTN_UP 4
#define PIN_BTN_DOWN 3
#define PIN_BTN_LEFT 5
#define PIN_BTN_RIGHT 6

#define NUM_TILES 10
#define TILE_SIZE 3
#define NUM_LEDS NUM_TILES*TILE_SIZE

#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define INITIAL_BRIGHTNESS 128
#define BRIGHTNESS_STEP 16
