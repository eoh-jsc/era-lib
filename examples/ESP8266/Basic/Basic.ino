#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    ERa.begin(ssid, pass);
}

void loop() {
    ERa.run();
}
