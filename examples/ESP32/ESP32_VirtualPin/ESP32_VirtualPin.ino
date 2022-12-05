/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
// #define ERA_DEBUG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"
// Force using virtual pin
#define FORCE_VIRTUAL_PIN

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

#define YOU_RELAY_PIN  32

ERaTimer timer;

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

/* This function is called every time the Virtual Pin 0 state change */
ERA_WRITE(ERA0) {
    /* Get value from Virtual Pin 0 and write Pin 32 */
    uint8_t value = (int)param;
    digitalWrite(YOU_RELAY_PIN, value ? HIGH : LOW);
}

/* This function send uptime every second to Virtual Pin 1 */
void timerEvent() {
    ERa.virtualWrite(ERA1, millis() / 1000L);
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    timer.setInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
    timer.run();
}
