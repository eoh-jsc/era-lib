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
    Serial.print("Pin 15 write value: ");
    Serial.println(pinValue);
    return false;
}

/* This is a callback function that is called when the pin 0 is read. */
ERA_PIN_READ(V0) {
    int pinValue = param.getInt();
    Serial.print("Pin 0 value is: ");
    Serial.println(pinValue);
}

/* This is a callback function that is called when the ERa is connected to the server. */
ERA_CONNECTED() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("ERa connected!");
}

/* This is a callback function that is called when the ERa is disconnected to the server. */
ERA_DISCONNECTED() {
    digitalWrite(LED_PIN, LOW);
    Serial.println("ERa disconnected!");
}

/* This function is called every second by the timer.
* It writes the current time in seconds to the Virtual pin 1 and
* updates the report with the current value of the button. */
void timerEvent() {
    ERa.virtualWrite(V1, millis() / 1000L);
    reportIt.updateReport(digitalRead(BUTTON_PIN));
}

/* It writes the current value of the button to the configId Virtual pin 2. */
void reportEvent() {
    ERa.virtualWrite(V2, digitalRead(BUTTON_PIN));
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Setup pin mode pin */
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    /* Set board id */
    // ERa.setBoardID("Board_1");
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
