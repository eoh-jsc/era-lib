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

/* Define setting button */
// #define BUTTON_PIN              0

#if defined(BUTTON_PIN)
    // Active low (false), Active high (true)
    #define BUTTON_INVERT       false
    #define BUTTON_HOLD_TIMEOUT 5000UL

    // This directive is used to specify whether the configuration should be erased.
    // If it's set to true, the configuration will be erased.
    #define ERA_ERASE_CONFIG    false
#endif

#include <Arduino.h>

/* GPIO and Virtual Pin */
#include <ERaSimpleEsp32.hpp>
/* GPIO, Virtual Pin and Modbus */
// #include <ERaSimpleMBEsp32.hpp>

#include <Automation/ERaSmart.hpp>
#include <Time/ERaEspTime.hpp>
#if defined(BUTTON_PIN)
    #include <pthread.h>
    #include <ERa/ERaButton.hpp>
#endif

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

WiFiClient mbTcpClient;

ERaEspTime syncTime;
ERaSmart smart(ERa, syncTime);

#include "ESP32_WiFi_Gsm.hpp"

#if defined(BUTTON_PIN)
    ERaButton button;
    pthread_t pthreadButton;

    static void* handlerButton(void* args) {
        for (;;) {
            button.run();
            ERaDelay(10);
        }
        pthread_exit(NULL);
    }

#if ERA_VERSION_NUMBER >= ERA_VERSION_VAL(1, 6, 0)
    static void eventButton(uint16_t pin, ButtonEventT event) {
        if (event != ButtonEventT::BUTTON_ON_HOLD) {
            return;
        }
        ERa.switchToConfig(ERA_ERASE_CONFIG);
        (void)pin;
    }
#elif ERA_VERSION_NUMBER >= ERA_VERSION_VAL(1, 2, 0)
    static void eventButton(uint8_t pin, ButtonEventT event) {
        if (event != ButtonEventT::BUTTON_ON_HOLD) {
            return;
        }
        ERa.switchToConfig(ERA_ERASE_CONFIG);
        (void)pin;
    }
#else
    static void eventButton(ButtonEventT event) {
        if (event != ButtonEventT::BUTTON_ON_HOLD) {
            return;
        }
        ERa.switchToConfig(ERA_ERASE_CONFIG);
    }
#endif

    void initButton() {
        pinMode(BUTTON_PIN, INPUT);
        button.setButton(BUTTON_PIN, digitalRead, eventButton,
                        BUTTON_INVERT).onHold(BUTTON_HOLD_TIMEOUT);
        pthread_create(&pthreadButton, NULL, handlerButton, NULL);
    }
#endif

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

#if defined(BUTTON_PIN)
    /* Initializing button. */
    initButton();
    /* Enable read/write WiFi credentials */
    ERa.setPersistent(true);
#endif

    /* Init GSM */
    initGsm();

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Set Non-Blocking mode */
    ERa.setNonBlocking(true);

    /* Set API task size. If this function is enabled,
       the core API will run on a separate task after disconnecting from the server
       (suitable for edge automation).*/
    // ERa.setTaskSize(ERA_API_TASK_SIZE, true);

    /* Set scan WiFi. If activated, the board will scan
       and connect to the best quality WiFi. */
    ERa.setScanWiFi(true);

    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
