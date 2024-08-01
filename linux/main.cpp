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
#include <ERaOptionsArgs.hpp>

static const char* auth;
static const char* boardID;
static const char* host;
static uint16_t port;
static const char* user;
static const char* pass;

ERaSocket mbTcpClient;

/* This function will run every time ERa is connected */
ERA_CONNECTED() {
    printf("ERa connected!\r\n");
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED() {
    printf("ERa disconnected!\r\n");
}

#if defined(ERA_ZIGBEE)
    void setupZigbee() {
        /* For reset and skip bootloader */
        ERa.setZigbeeDtrRts(DTR_PIN, RTS_PIN);
    }
#else
    void setupZigbee() {
    }
#endif

void setup() {
    /* Setup Zigbee */
    setupZigbee();

    /* Setup Client for Modbus TCP/IP */
    ERa.setModbusClient(mbTcpClient);

    ERa.setAppLoop(false);
    ERa.setBoardID(boardID);
    ERa.begin(auth, host, port, user, pass);
    ERa.addInterval(1000L, [](void) {
        printf("Uptime: %d\r\n", ERaMillis() / 1000L);
    });
}

void loop() {
    ERa.run();
}

int main(int argc, char* argv[]) {
    processArgs(argc, argv, auth, boardID,
                host, port, user, pass);

    setup();
    while (1) {
        loop();
    }

    return 0;
}
