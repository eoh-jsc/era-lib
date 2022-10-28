#include <ERaSimpleMBWiringPi.hpp>
#include <ERa/ERaTimer.hpp>
#include <ERaOptionsArgs.hpp>

static const char* auth;
static const char* host;
static uint16_t port;

ERaTimer timer;

void setup() {
    ERa.begin(auth, host, port, auth, auth);
    timer.setInterval(1000L, []() {
        printf("Uptime: %d\n", ERaMillis() / 1000L);
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