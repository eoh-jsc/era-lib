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
// #define ERA_SERIAL stdout

// Enable Modbus
// #include <ERaSimpleMBLinux.hpp>

// Enable Modbus and Zigbee
#include <ERaLinux.hpp>

#include <ERa/ERaTimer.hpp>
#include <ERaOptionsArgs.hpp>

static const char* auth;
static const char* host;
static uint16_t port;

ERaTimer timer;

void setup() {
    ERa.begin(auth, host, port, auth, auth);
    timer.setInterval(1000L, []() {
        printf("Uptime: %d\r\n", ERaMillis() / 1000L);
    });
}

void loop() {
    ERa.run();
    timer.run();
}

int main(int argc, char *argv[]) {
    processArgs(argc, argv, auth, host, port);

    setup();
    while (1) {
        loop();
    }

    return 0;
}