#define TINY_GSM_MODEM_ESP8266
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERaSimpleStm32WiFi.hpp>
#include <ERa/ERaTimer.hpp>
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

    ERa.begin(modem, ssid, pass);
}

void loop() {
    ERa.run();
}
