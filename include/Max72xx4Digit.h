#pragma once

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <RTClib.h>

#include "ClockFontRD.h"

#define PIN_CS D4
#define NUMBER_OF_HORIZONTAL_DISPLAYS   4
#define NUMBER_OF_VERTICAL_DISPLAYS     1
#define NIGHT_DISPLAY_INTENSITY         0
#define DAY_DISPLAY_INTENSITY           5

void SetupDisplay();
void DisplayMessage(String message);
void DisplayClock(DateTime dateTime);
void DisplayDate(DateTime dateTime);
void AdjustMax72XXBrightness(DateTime dateTime);
