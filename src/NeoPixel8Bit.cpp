#include "NeoPixel8Bit.h"

static CRGB NeopixelColor(CRGB::White);
static CRGB NeopixelLeds[NEOPIXEL_NUM_LEDS];

void SetupNeoPixel(){
    FastLED.addLeds<WS2812, NEOPIXEL_DATA_PIN, GRB>(NeopixelLeds, NEOPIXEL_NUM_LEDS).setCorrection(LEDColorCorrection::TypicalSMD5050);
    FastLED.setTemperature(CRGB(255, 150, 15));
    FastLED.setBrightness(255);

    TurnOffNeopixel();
}

void TurnOnNeopixel(){
    for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
    {
        NeopixelLeds[i] = NeopixelColor;
        FastLED.show();
    }
}

void TurnOffNeopixel(){
    for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
    {
        NeopixelLeds[i] = CRGB::Black;
        FastLED.show();
    }
}

void AdjustNeopixel(DateTime dateTime){
    int Hours = dateTime.hour();

    if (Hours > 17 || Hours < 6){
        TurnOnNeopixel();
    }
    else{  
        TurnOffNeopixel();
    }
}
