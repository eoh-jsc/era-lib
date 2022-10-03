#include <Arduino.h>
#include <MVP.hpp>
#include <MVP/MVPTimer.hpp>

#define LED_PIN        33
#define BUTTON_PIN     32

const char auth[] = "MVP2706";
const char ssid[] = "WIFI_SSID";
const char pass[] = "WIFI_PASS";
const char host[] = "MQTT_HOST";
const uint16_t port = 1883;
const char user[] = "MQTT_USER";
const char password[] = "MQTT_PASS";

MVPTimer timer;
MVPReport report;
int reportId {0};

/* This is a callback function that is called when the pin 15 is written. */
MVP_PIN_WRITE(MVP15) {
    int pinValue = (int)param;
    Serial.print("Pin 15 write value: ");
    Serial.println(pinValue);
}

/* This is a callback function that is called when the pin 0 is read. */
MVP_PIN_READ(MVP0) {
    int pinValue = (int)param;
    Serial.print("Pin 0 value is: ");
    Serial.println(pinValue);
}

/* This is a callback function that is called when the MVP is connected to the server. */
MVP_CONNECTED() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("MVP Connected");
}

/* This is a callback function that is called when the MVP is disconnected to the server. */
MVP_DISCONNECTED() {
    digitalWrite(LED_PIN, LOW);
    Serial.println("MVP Disconnected");
}

/* This function is called every second by the timer.
* It writes the current time in seconds to the configId 2706 and
* updates the report with the current value of the button. */
void timerEvent() {
    mvp.configIdWrite(2706, millis() / 1000L);
    report.updateReport(reportId, digitalRead(BUTTON_PIN));
}

/* It writes the current value of the button to the configId 2706. */
void reportEvent() {
    mvp.configIdWrite(2706, digitalRead(BUTTON_PIN));
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    /* Initializing the MVP library. */
    mvp.begin(auth, ssid, pass,
            host, port, user, password);

    /* Setting the timer to call the timerEvent function every second. */
    timer.setInterval(1000L, timerEvent);

    /* Setting up a report */
    reportId = report.setReporting(1000L, 60000L, 1.0f, reportEvent);
}

void loop() {
    mvp.run();
    timer.run();
    report.run();
}
