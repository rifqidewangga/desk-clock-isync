#include <Arduino.h>

#include "WiFiSetup.h"
#include "NeoPixel8Bit.h"
#include "Max72xx4Digit.h"

#include <Ticker.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>

void SyncRTC();
String GetDateTimeString(DateTime dateTime);

// RTC DS3231
RTC_DS3231 RTC;
DateTime TimeNowRTC;

// Define NTP Client to get time
const long timeZoneUTC = 7; // UTC +7
const long utcOffsetInSeconds = 3600 * timeZoneUTC;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Time Management
void DisplayClockTicker() {
    TimeNowRTC = RTC.now();

    AdjustMax72XXBrightness(TimeNowRTC);
    DisplayClock(TimeNowRTC);
    
    AdjustNeopixel(TimeNowRTC);
}
Ticker ScreenUpdateTicker(DisplayClockTicker, 1000, 0, MILLIS);

Ticker SyncRTCTicker(SyncRTC, 1000, 0, MILLIS);
const int32_t UpperTimeDeltaThresholdToSyncS = 300; // Seconds
const int32_t LowerTimeDeltaThresholdToSyncS = 10; // Seconds

void setup() {
    Serial.begin(115200);

    // RTC Setup
    if (! RTC.begin()) {
        Serial.println("\n\nCouldn't find RTC, check your connection!");
        while (true){/*Don't proceed if RTC unavailable*/}
    }
    else{
        Serial.println("\n\nRTC Connected");
    }
    TimeNowRTC = RTC.now();

    SetupDisplay();
    SetupNeoPixel();

    ScreenUpdateTicker.start();
    SyncRTCTicker.start();

    WiFiSetup(WIFI_MANAGER_TIMEOUT_S);

    timeClient.begin();
    timeClient.update();

    AdjustMax72XXBrightness(TimeNowRTC);
    DisplayDate(TimeNowRTC);
}

void loop() {
    ScreenUpdateTicker.update();
    SyncRTCTicker.update();
}

String GetDateTimeString(DateTime dateTime) {
    String network_date_time = String(dateTime.year());
    network_date_time += "-";
    network_date_time += String(dateTime.month());
    network_date_time += "-";
    network_date_time += String(dateTime.day());
    network_date_time += " ";

    // Time
    network_date_time += String(dateTime.hour());
    network_date_time += ":";
    if(dateTime.minute() < 10)
        network_date_time += "0";
    network_date_time += String(dateTime.minute());
    network_date_time += ":";
    if(dateTime.second() < 10)
        network_date_time += "0";
    network_date_time += String(dateTime.second());

    return network_date_time;
}

void SyncRTC(){
    if(!WiFi.isConnected()){
        Serial.println("Internet Connection Unavailable. Failed to Sync RTC to Server.");
        return;
    }

    if (timeClient.update()){
        unsigned long epochTime = timeClient.getEpochTime();

        DateTime internetTime(epochTime);
        Serial.print("Internet Time: ");
        Serial.println(GetDateTimeString(internetTime));

        Serial.print("RTC Time: ");
        Serial.println(GetDateTimeString(RTC.now()));

        // Only sync if the time discrepancy is less than 600 seconds. 
        // Keep RTC as primary time keeper and only sync if the time delta is reasonable.
        // Don't assume internet time always correct
        int32_t deltaTimeSAbs = internetTime.unixtime() - RTC.now().unixtime();

        if (abs(deltaTimeSAbs) < UpperTimeDeltaThresholdToSyncS){
            if (abs(deltaTimeSAbs) < 5){
                Serial.print("RTC Not Sync, time delta only ");
                Serial.print(abs(deltaTimeSAbs));
                Serial.println(" seconds\n");
                return;
            }

            RTC.adjust(internetTime);
            Serial.println("RTC Synced\n");
        }
        else{
            Serial.println("Internet time might be wrong, skipping RTC sync\n");
        }
    }
}
