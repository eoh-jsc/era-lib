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
// #define ERA_DEBUG
// #define ERA_SERIAL Serial

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

/* Define setting button */
// #define BUTTON_PIN              0

#if defined(BUTTON_PIN)
    // Active low (false), Active high (true)
    #define BUTTON_INVERT       false
    #define BUTTON_HOLD_TIMEOUT 5000UL

    // This directive is used to specify whether the configuration should be erased.
    // If it's set to true, the configuration will be erased.
    #define ERA_ERASE_CONFIG    true
#endif

#include <Arduino.h>
#include <ERa.hpp>
#if defined(BUTTON_PIN)
    #include <GTimer.h>
    #include <ERa/ERaButton.hpp>
#endif

WiFiClient mbTcpClient;

#if defined(BUTTON_PIN)
    ERaButton button;
    const uint32_t timerIdButton {0};

    static void handlerButton(uint32_t data) {
        button.run();
        (void)data;
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
        button.setButton(BUTTON_PIN, digitalReadArduino, eventButton,
                        BUTTON_INVERT).onHold(BUTTON_HOLD_TIMEOUT);
        GTimer.begin(timerIdButton, (100 * 1000), handlerButton);
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
#endif

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Setup Client for Modbus TCP/IP */
    ERa.setModbusClient(mbTcpClient);

    /* White labeling App (use this ONLY if you have a branded ERa App) */
    // ERa.setVendorName("MyORG");
    // ERa.setVendorPrefix("MyPrefix");

    /* Set scan WiFi. If activated, the board will
       check WiFi before connect. */
    ERa.setScanWiFi(true);

    /* Initializing the ERa library. */
    ERa.begin();

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
