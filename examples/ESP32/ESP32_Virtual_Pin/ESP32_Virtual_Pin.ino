/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************
  Dashboard setup:
    Virtual Pin setup:
        V0, type: number
        V1, type: string

    Button widget setup:
        Datastream: V0
        Action on: the action of turning on V0
        Action off: the action of turning off V0

    Text Box widget setup:
        Datastream: V1
        Action: the action of V1
 *************************************************************/

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
// #define ERA_DEBUG
// #define ERA_SERIAL Serial

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>

#define LED_PIN  2

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

/* This function is called every time the Virtual Pin 0 state change */
ERA_WRITE(V0) {
    /* Get value from Virtual Pin 0 and write LED */
    uint8_t value = param.getInt();
    digitalWrite(LED_PIN, value ? HIGH : LOW);

    // Send the LED status back
    ERa.virtualWrite(V0, digitalRead(LED_PIN));
}

/* This function will execute each time from the Text Box to your chip */
ERA_WRITE(V1) {
    /* Get value from Virtual Pin 1 */
    ERaString estr = param.getString();

    // If you type "on", turn on LED
    // If you type "off", turn off LED
    if (estr == "on") {
        digitalWrite(LED_PIN, HIGH);
    }
    else if (estr == "off") {
        digitalWrite(LED_PIN, LOW);
    }

    // Send it back
    ERa.virtualWrite(V1, estr);
    // Send the LED status back
    ERa.virtualWrite(V0, digitalRead(LED_PIN));
}

/* This function will run every time ERa is connected */
ERA_CONNECTED() {
    ERA_LOG("ERa", "ERa connected!");
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED() {
    ERA_LOG("ERa", "ERa disconnected!");
}

/* This function send uptime every second to Virtual Pin 1 */
void timerEvent() {
    ERa.virtualWrite(V1, ERaMillis() / 1000L);
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Setup pin mode led pin */
    pinMode(LED_PIN, OUTPUT);

    /* Set board id */
    // ERa.setBoardID("Board_1");
    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
