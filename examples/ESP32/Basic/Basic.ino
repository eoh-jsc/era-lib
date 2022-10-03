#define MVP_AUTH_TOKEN "MVP2706"

#include <Arduino.h>
#include <MVP.hpp>
#include <MVP/MVPTimer.hpp>

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    mvp.begin(ssid, pass);
}

void loop() {
    mvp.run();
}
