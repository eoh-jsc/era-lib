#define MVP_AUTH_TOKEN "MVP2706"

#include <Arduino.h>
#include <MVP.hpp>
#include <MVP/MVPTimer.hpp>

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    mvp.begin();
}

void loop() {
    mvp.run();
}
