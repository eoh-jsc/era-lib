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

/* Note: Preferences Arduino IDE
    https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
*/

// Enable debug console
// #define ERA_DEBUG
// #define ERA_SERIAL Serial

/* Select your modem */
#define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_ESP32

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <SoftwareSerial.h>

/* You should increase Serial fifo buffer size */
SoftwareSerial SerialWiFi(17, 16);
TinyGsm modem(SerialWiFi);

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

const int rstPin = 7;

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
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Set GSM module baud rate */
    SerialWiFi.begin(115200);

    /* Set board id */
    // ERa.setBoardID("Board_1");
    /* Initializing the ERa library. */
    ERa.begin(modem, ssid, pass, rstPin);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
