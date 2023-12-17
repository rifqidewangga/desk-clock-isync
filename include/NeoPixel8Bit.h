#pragma once

#include <FastLED.h>
#include <RTClib.h>

// NeoPixel
#define NEOPIXEL_NUM_LEDS 8
#define NEOPIXEL_DATA_PIN D3

void SetupNeoPixel();
void TurnOnNeopixel();
void TurnOffNeopixel();
void AdjustNeopixel(DateTime dateTime);
