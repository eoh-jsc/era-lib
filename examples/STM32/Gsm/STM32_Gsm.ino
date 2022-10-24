/* Select your apn */
#define APN_VIETTEL
// #define APN_VINAPHONE
// #define APN_MOBIFONE

/* Select your modem */
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM900

#define MVP_AUTH_TOKEN "MVP2706"

#include <Arduino.h>
#include <MVPSimpleStm32Gsm.hpp>
#include <MVP/MVPTimer.hpp>
#include <TinyGsmClient.h>

HardwareSerial SerialGsm(PC7, PC6);
TinyGsm modem(SerialGsm);

#if defined(APN_VIETTEL)
    const char apn[] = "v-internet";
    const char user[] = "";
    const char pass[] = "";
#elif defined(APN_VINAPHONE)
    const char apn[] = "m3-world";
    const char user[] = "mms";
    const char pass[] = "mms";
#elif defined(APN_MOBIFONE)
    const char apn[] = "m-wap";
    const char user[] = "mms";
    const char pass[] = "mms";
#else
    const char apn[] = "internet";
    const char user[] = "";
    const char pass[] = "";
#endif

const int pwrPin = PB7;

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Set GSM module baud rate */
    SerialGsm.begin(115200);

    mvp.begin(modem, apn, user, pass, pwrPin);
}

void loop() {
    mvp.run();
}
