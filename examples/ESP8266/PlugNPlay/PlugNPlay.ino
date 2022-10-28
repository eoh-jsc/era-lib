#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    ERa.begin();
}

void loop() {
    ERa.run();
}
