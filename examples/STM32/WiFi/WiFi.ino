#define TINY_GSM_MODEM_ESP8266
#define MVP_AUTH_TOKEN "MVP2706"

#include <Arduino.h>
#include <MVPSimpleStm32WiFi.hpp>
#include <MVP/MVPTimer.hpp>
#include <TinyGsmClient.h>

HardwareSerial SerialWiFi(PC7, PC6);
TinyGsm modem(SerialWiFi);

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Set GSM module baud rate */
    SerialWiFi.begin(115200);

    mvp.begin(modem, ssid, pass);
}

void loop() {
    mvp.run();
}
