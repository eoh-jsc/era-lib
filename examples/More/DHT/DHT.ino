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
        V1, type: number

    This example need Adafruit DHT sensor libraries:
        https://github.com/adafruit/Adafruit_Sensor
        https://github.com/adafruit/DHT-sensor-library
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
#include <DHT.h>

/* Digital pin connected to sensor */
#define DHT_PIN 2

/* Uncomment type using! */
#define DHT_TYPE DHT11
// #define DHT_TYPE DHT21
// #define DHT_TYPE DHT22

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

DHT dht(DHT_PIN, DHT_TYPE);

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
    // Read humidity and temperature
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        ERA_LOG("DHT", "Failed to read DHT!");
        return;
    }

    // Send the humidity and temperature value to ERa
    ERa.virtualWrite(V0, humidity);
    ERa.virtualWrite(V1, temperature);

    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

    /* Set board id */
    // ERa.setBoardID("Board_1");

    /* Initialize DHT */
    dht.begin();

    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
}
