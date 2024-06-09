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
// #define ERA_SERIAL Serial

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <Time/ERaEspTime.hpp>

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

/* NTP Time */
ERaEspTime syncTime;
TimeElement_t ntpTime;

/* This function will run every time ERa is connected */
ERA_CONNECTED() {
    ERA_LOG("ERa", "ERa connected!");
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED() {
    ERA_LOG("ERa", "ERa disconnected!");
}

/* This function print uptime every second */
void timerEvent() {
    syncTime.getTime(ntpTime);
    ERA_LOG(ERA_PSTR("NTP"), ERA_PSTR("%02d:%02d:%02dT%02d-%02d-%04dW%d"),
                             ntpTime.hour, ntpTime.minute, ntpTime.second,
                             ntpTime.day, ntpTime.month, ntpTime.year + 1970, ntpTime.wDay);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Set timezone +7 */
    syncTime.setTimeZone(7);
    syncTime.begin();

    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
