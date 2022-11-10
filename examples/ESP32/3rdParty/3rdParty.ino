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

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

#define LED_PIN        33
#define BUTTON_PIN     32

// You should get Auth Token in the ERa App or ERa Dashboard
const char auth[] = "ERA2706";
const char ssid[] = "WIFI_SSID";
const char pass[] = "WIFI_PASS";
const char host[] = "MQTT_HOST";
const uint16_t port = 1883;
const char user[] = "MQTT_USER";
const char password[] = "MQTT_PASS";

ERaTimer timer;
ERaReport report;
int reportId {0};

/* This is a callback function that is called when the pin 15 is written. */
ERA_PIN_WRITE(ERA15) {
    int pinValue = (int)param;
    Serial.print("Pin 15 write value: ");
    Serial.println(pinValue);
}

/* This is a callback function that is called when the pin 0 is read. */
ERA_PIN_READ(ERA0) {
    int pinValue = (int)param;
    Serial.print("Pin 0 value is: ");
    Serial.println(pinValue);
}

/* This is a callback function that is called when the ERa is connected to the server. */
ERA_CONNECTED() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("ERa Connected");
}

/* This is a callback function that is called when the ERa is disconnected to the server. */
ERA_DISCONNECTED() {
    digitalWrite(LED_PIN, LOW);
    Serial.println("ERa Disconnected");
}

/* This function is called every second by the timer.
* It writes the current time in seconds to the configId 2706 and
* updates the report with the current value of the button. */
void timerEvent() {
    ERa.configIdWrite(2706, millis() / 1000L);
    report.updateReport(reportId, digitalRead(BUTTON_PIN));
}

/* It writes the current value of the button to the configId 2706. */
void reportEvent() {
    ERa.configIdWrite(2706, digitalRead(BUTTON_PIN));
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    /* Initializing the ERa library. */
    ERa.begin(auth, ssid, pass,
            host, port, user, password);

    /* Setting the timer to call the timerEvent function every second. */
    timer.setInterval(1000L, timerEvent);

    /* Setting up a report */
    reportId = report.setReporting(1000L, 60000L, 1.0f, reportEvent);
}

void loop() {
    ERa.run();
    timer.run();
    report.run();
}
