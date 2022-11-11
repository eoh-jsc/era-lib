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

// You should get Auth Token in the ERa App or ERa Dashboard
#define TINY_GSM_MODEM_ESP8266
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

HardwareSerial SerialWiFi(PC7, PC6);
TinyGsm modem(SerialWiFi);

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Set GSM module baud rate */
    SerialWiFi.begin(115200);

    ERa.begin(modem, ssid, pass);
}

void loop() {
    ERa.run();
}
