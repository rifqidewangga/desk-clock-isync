#include "WiFiSetup.h"

void WiFiSetup(unsigned long timeout_s){
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(false);
    wifiManager.setTimeout(timeout_s);
    wifiManager.autoConnect();
}
