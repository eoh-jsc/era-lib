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

#include <Arduino.h>
#include <ERa.hpp>

#define RELAY_PIN      25
#define LED_PIN        33
#define BUTTON_PIN     32

// You should get Auth Token in the ERa App or ERa Dashboard
// and not share this token with anyone.
const char auth[] = "ERA2706";
const char ssid[] = "WIFI_SSID";
const char pass[] = "WIFI_PASS";
const char host[] = "MQTT_HOST";
const uint16_t port = 1883;
const char username[] = "MQTT_USER";
const char password[] = "MQTT_PASS";

ERaReport report;
ERaReport::iterator reportIt;

/* This function is called every time the Virtual Pin 0 state change */
ERA_WRITE(V0) {
    /* Get value from Virtual Pin 0 and write Pin 32 */
    uint8_t value = param.getInt();
    digitalWrite(RELAY_PIN, value ? HIGH : LOW);
    ERa.virtualWrite(V0, digitalRead(RELAY_PIN));
}

/* This is a callback function that is called when the pin 15 is written. */
ERA_PIN_WRITE(V15) {
    int pinValue = param.getInt();
    ERA_LOG("Pin", "Pin 15 write value: %d", pinValue);
    return false;
}

/* This is a callback function that is called when the pin 0 is read. */
ERA_PIN_READ(V0) {
    int pinValue = param.getInt();
    ERA_LOG("Pin", "Pin 0 value is: %d", pinValue);
}

/* This is a callback function that is called when the ERa is connected to the server. */
ERA_CONNECTED() {
    digitalWrite(LED_PIN, HIGH);
    ERA_LOG("ERa", "ERa connected!");
}

/* This is a callback function that is called when the ERa is disconnected to the server. */
ERA_DISCONNECTED() {
    digitalWrite(LED_PIN, LOW);
    ERA_LOG("ERa", "ERa disconnected!");
}

/* This function is called every second by the timer.
* It writes the current time in seconds to the Virtual pin 1 and
* updates the report with the current value of the button. */
void timerEvent() {
    unsigned long uptime = ERaMillis() / 1000L;
    ERa.virtualWrite(V1, uptime);
    reportIt.updateReport(digitalRead(BUTTON_PIN));
    ERA_LOG("Timer", "Uptime: %d", uptime);
}

/* It writes the current value of the button to the configId Virtual pin 2. */
void reportEvent() {
    ERa.virtualWrite(V2, digitalRead(BUTTON_PIN));
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Setup pin mode pin */
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Set scan WiFi. If activated, the board will scan
       and connect to the best quality WiFi. */
    ERa.setScanWiFi(true);

    /* Initializing the ERa library. */
    ERa.begin(auth, ssid, pass,
            host, port, username, password);

    /* Setting the timer to call the timerEvent function every second. */
    ERa.addInterval(1000L, timerEvent);

    /* Setting up a report */
    reportIt = report.setReporting(1000L, 60000L, 1.0f, reportEvent);
}

void loop() {
    ERa.run();
    report.run();
}
