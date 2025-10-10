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
// Recommended: use CORE_DEBUG_LEVEL = 3
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
// and not share this token with anyone.
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>

/* GPIO and Virtual Pin */
#include <ERaSimpleEsp32Gsm.hpp>
/* GPIO, Virtual Pin and Modbus */
// #include <ERaSimpleMBEsp32Gsm.hpp>
#include <Automation/ERaSmart.hpp>
#include <Time/ERaBaseTime.hpp>

/* GPIO and Virtual Pin */
HardwareSerial SerialGsm(1);
/* GPIO, Virtual Pin and Modbus */
// HardwareSerial SerialGsm(2);
TinyGsm modem(SerialGsm);

ERaBaseTime syncTime;
ERaSmart smart(ERa, syncTime);

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

void getTimeFromGsm() {
    int year, month, day, hour, minute, second {0};
    bool status = modem.getNetworkTime(
        &year, &month, &day, &hour, &minute, &second, nullptr
    );
    if (status) {
        long offset = syncTime.getTimezoneOffset();
        syncTime.setTime(hour, minute, second, day, month, year, -offset);
    }
}

/* This function is triggered whenever an SMS is sent */
ERA_WRITE_SMS() {
    ERA_LOG("ERa", "Write SMS to %s: %s", to, message);
    return false;
}

/* This function will run every time ERa is connected */
ERA_CONNECTED() {
    ERA_LOG("ERa", "ERa connected!");
    getTimeFromGsm();
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED() {
    ERA_LOG("ERa", "ERa disconnected!");
}

/* This function print uptime every second */
void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

void setRTC(unsigned long time) {
    const timeval epoch = {(time_t)time, 0};
    settimeofday(&epoch, 0);
}

unsigned long getRTC() {
    if (ERa.connected()) {
        getTimeFromGsm();
    }
    return time(NULL);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Set time callback */
    syncTime.setSetTimeCallback(setRTC);
    syncTime.setGetTimeCallback(getRTC);

    /* Set GSM module baud rate */
    SerialGsm.begin(115200, SERIAL_8N1, gsmRxPin, gsmTxPin);

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Set API task size. If this function is enabled,
       the core API will run on a separate task after disconnecting from the server
       (suitable for edge automation).*/
    // ERa.setTaskSize(ERA_API_TASK_SIZE, true);

    /* Initializing the ERa library. */
    ERa.begin(modem, apn, user, pass, pwrPin);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
