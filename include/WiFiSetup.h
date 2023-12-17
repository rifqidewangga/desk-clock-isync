#pragma once

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

#define WIFI_MANAGER_TIMEOUT_S 30

void WiFiSetup(unsigned long timeout_s);