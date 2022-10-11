#include <Arduino.h>
#ifdef ESP32
    #include <DNSServer.h>
    #include <WebServer.h>
    #include <WiFi.h>
#else
    #include <DNSServer.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266WiFi.h>
#endif
#include <WiFiManager.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Ticker.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>

#include "clock_font_rd.h"

void WiFiSetup(unsigned long timeout_s);
void WiFiMonitor();

void SetupDisplay();
void DisplayMessage(String message);
void DisplayClock();
void AdjustBrightness();
void DisplayDate();

void SyncRTC();
String GetDateTimeString(unsigned long epochTime);

// RTC DS3231
RTC_DS3231 RTC;
DateTime TimeNowRTC;

// Display
#define PIN_CS D4
#define NUMBER_OF_HORIZONTAL_DISPLAYS   4
#define NUMBER_OF_VERTICAL_DISPLAYS     1
#define NIGHT_DISPLAY_INTENSITY         0
#define DAY_DISPLAY_INTENSITY           5
bool DisplayDot = true;
Max72xxPanel matrix = Max72xxPanel(PIN_CS, NUMBER_OF_HORIZONTAL_DISPLAYS, NUMBER_OF_VERTICAL_DISPLAYS);

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// Define NTP Client to get time
const long timeZoneUTC = 7; // UTC +7
const long utcOffsetInSeconds = 3600 * timeZoneUTC;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Time Management
Ticker ScreenUpdateTicker(DisplayClock, 1000, 0, MILLIS);
Ticker SyncRTCTicker(SyncRTC, 1000, 0, MILLIS);

int wait = 100; // In milliseconds
int spacer = 1;
int width  = 5 + spacer; // The font width is 5 pixels

void setup() {
    SetupDisplay();

    ScreenUpdateTicker.start();
    SyncRTCTicker.start();

    Serial.begin(115200);

    // RTC Setup
    if (! RTC.begin()) {
        Serial.println("\n\nCouldn't find RTC");
    }
    else{
        Serial.println("\n\nRTC Connected");
    }

    WiFiSetup(30);

    timeClient.begin();
    timeClient.update();
    TimeNowRTC = RTC.now();

    DisplayDate();
}

void loop() {
    ScreenUpdateTicker.update();
    SyncRTCTicker.update();
}

void SetupDisplay(){
    matrix.setIntensity(DAY_DISPLAY_INTENSITY);
    matrix.setRotation(0, 3);
    matrix.setRotation(1, 3);
    matrix.setRotation(2, 3);
    matrix.setRotation(3, 3);

    matrix.setPosition(0, 3, 0);
    matrix.setPosition(1, 2, 0);
    matrix.setPosition(2, 1, 0);
    matrix.setPosition(3, 0, 0);

    matrix.fillScreen(LOW);
    matrix.write();
}

void DisplayMessage(String message){
    for ( unsigned int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
        //matrix.fillScreen(LOW);
        unsigned int letter = i / width;
        int x = (matrix.width() - 1) - i % width;
        int y = (matrix.height() - 8) / 2; // center the text vertically

        while ( x + width - spacer >= 0 && letter >= 0 ) {
            if ( letter < message.length() ) {
                matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
            }
            letter--;
            x -= width;
        }

        matrix.write(); // Send bitmap to display
        delay(wait/2);
    }
}

void AdjustBrightness(){
    int Hours = TimeNowRTC.hour();

    if (Hours > 17 || Hours < 6)
        matrix.setIntensity(NIGHT_DISPLAY_INTENSITY);
    else
        matrix.setIntensity(DAY_DISPLAY_INTENSITY);
}

String GetDateTimeString(DateTime dateTime)
{
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

void DisplayClock(){
    // Get Time From Local RTC
    TimeNowRTC = RTC.now();
    int tempDigit;

    matrix.fillScreen(LOW);

    // First Digit
    tempDigit = TimeNowRTC.hour() / 10;
    if(tempDigit > 0){
        matrix.drawBitmap(0, 0, CF_LEFT[tempDigit], 8, 8, 1);
    }

    // Second Digit
    tempDigit = TimeNowRTC.hour() % 10;
    matrix.drawBitmap(7, 0, CF_LEFT[tempDigit], 8, 8, 1);

    // Display Colon
    if(DisplayDot){
        matrix.fillRect(15, 1, 2, 2, 1);
        matrix.fillRect(15, 6, 2, 2, 1);
    }

    // Third Digit
    tempDigit = TimeNowRTC.minute() / 10;
    matrix.drawBitmap(17, 0, CF_RIGHT[tempDigit], 8, 8, 1);

    // Fourth Digit
    tempDigit = TimeNowRTC.minute() % 10;
    matrix.drawBitmap(24, 0, CF_RIGHT[tempDigit], 8, 8, 1);

    AdjustBrightness();
    matrix.write(); // Send bitmap to display

    if (DisplayDot == true){
        DisplayDot = false;
    }
    else{
        DisplayDot = true;
    }
}

void DisplayDate(){
    String FullDate;

    FullDate += weekDays[TimeNowRTC.dayOfTheWeek()];
    FullDate += ", ";
    FullDate += TimeNowRTC.day();
    FullDate += " ";
    FullDate += months[TimeNowRTC.month() - 1];
    FullDate += " ";
    FullDate += TimeNowRTC.year();

    DisplayMessage(FullDate);
}

void SyncRTC(){
    if(!WiFi.isConnected()){
        Serial.println("Internet Connection Unavailable. Failed to Sync RTC to Server.");
        return;
    }

    if (timeClient.update()){
        unsigned long epochTime = timeClient.getEpochTime();

        DateTime internetTime(epochTime);

        RTC.adjust(internetTime);
        Serial.println("RTC Synced");

        Serial.println(GetDateTimeString(internetTime));
    }
}

void WiFiSetup(unsigned long timeout_s){
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(false);
    wifiManager.setTimeout(timeout_s);
    wifiManager.autoConnect();
}
