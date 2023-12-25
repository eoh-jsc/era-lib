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

/* Select your apn */
#define APN_VIETTEL
// #define APN_VINAPHONE
// #define APN_MOBIFONE

/* Select your modem */
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM7600

/* For override Modbus uart pin */
// #define MODBUS_RXD_Pin 5
// #define MODBUS_TXD_Pin 4

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>

/* GPIO and Virtual Pin */
#include <ERaSimpleEsp32Gsm.hpp>
/* GPIO, Virtual Pin and Modbus */
// #include <ERaSimpleMBEsp32Gsm.hpp>

/* GPIO and Virtual Pin */
HardwareSerial SerialGsm(1);
/* GPIO, Virtual Pin and Modbus */
// HardwareSerial SerialGsm(2);
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

const int gsmRxPin = 18;
const int gsmTxPin = 17;
const int pwrPin = 1;

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
    SerialGsm.begin(115200, SERIAL_8N1, gsmRxPin, gsmTxPin);

    /* Set board id */
    // ERa.setBoardID("Board_1");
    /* Initializing the ERa library. */
    ERa.begin(modem, apn, user, pass, pwrPin);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
