#ifndef INC_ERA_WIFI_GSM_HPP_
#define INC_ERA_WIFI_GSM_HPP_

#define byte uint8_t
#include <TinyGsmClient.hpp>
#undef byte

#include <Utility/ERaPeriod.hpp>

#if defined(APN_VIETTEL)
    const char apn[] = "v-internet";
    const char usergsm[] = "";
    const char passgsm[] = "";
#elif defined(APN_VINAPHONE)
    const char apn[] = "m3-world";
    const char usergsm[] = "mms";
    const char passgsm[] = "mms";
#elif defined(APN_MOBIFONE)
    const char apn[] = "m-wap";
    const char usergsm[] = "mms";
    const char passgsm[] = "mms";
#else
    const char apn[] = "internet";
    const char usergsm[] = "";
    const char passgsm[] = "";
#endif

static volatile bool isGsmConnected = false;
static const char* const TAG = "GSM";

/* GPIO and Virtual Pin */
HardwareSerial SerialGsm(1);
/* GPIO, Virtual Pin and Modbus */
// HardwareSerial SerialGsm(2);
TinyGsm modem(SerialGsm);
TinyGsmClient gsmClient(modem);

const int gsmRxPin = 16;
const int gsmTxPin = 17;
const int pwrPin = 15;

bool restartModem() {
    ERA_LOG(TAG, "Restarting modem...");
    digitalWrite(pwrPin, HIGH);
    delay(1000);
    digitalWrite(pwrPin, LOW);
    delay(500);
    ERaWatchdogFeed();
    if (!modem.begin()) {
        ERA_LOG(TAG, "Failed to restart modem!");
        return false;
    }
    ERaWatchdogFeed();
    if (!modem.waitForNetwork()) {
        ERA_LOG(TAG, "Failed to connect to network!");
        return false;
    }
    ERaWatchdogFeed();
    if (!modem.gprsConnect(apn, usergsm, passgsm)) {
        ERA_LOG(TAG, "Failed to connect to GPRS!");
        return false;
    }
    ERaWatchdogFeed();
    ERA_LOG(TAG, "Modem restarted successfully!");
    return true;
}

bool gsmConnect() {
    static bool restarted {false};
    if (!modem.testAT() || !restarted) {
        restarted = restartModem();
    }
    return modem.isNetworkConnected();
}

bool gsmConnected() {
    size_t connectLimit {0};
    ERA_EVERY_N_MILLIS(500) {
        isGsmConnected = gsmConnect();
        WiFi.reconnect();
        if (++connectLimit >= 5) {
            break;
        }
    }
    return isGsmConnected;
}

void getTimeFromGsm() {
    static bool status {false};
    ERA_EVERY_N_MILLIS(600000) {
        status = false;
    }
    if (status) {
        return;
    }
    int year, month, day, hour, minute, second {0};
    status = modem.getNetworkTime(
        &year, &month, &day, &hour, &minute, &second, nullptr
    );
    if (status) {
        long offset = syncTime.getTimezoneOffset();
        syncTime.setTime(hour, minute, second, day, month, year, -offset);
    }
}

void setRTC(unsigned long time) {
    const timeval epoch = {(time_t)time, 0};
    settimeofday(&epoch, 0);
}

unsigned long getRTC() {
    if (isGsmConnected && !WiFi.isConnected()) {
        getTimeFromGsm();
    }
    return time(NULL);
}

void initGsm() {
    pinMode(pwrPin, OUTPUT);
    digitalWrite(pwrPin, LOW);
    SerialGsm.begin(115200, SERIAL_8N1, gsmRxPin, gsmTxPin);

    /* Set time callback */
    syncTime.setSetTimeCallback(setRTC);
    syncTime.setGetTimeCallback(getRTC);

    /* Set GSM callbacks */
    ERa.setWiFiPersistent(true);
    ERa.setGsmCallbacks(gsmConnected, gsmClient);
}

/* This function is triggered whenever an SMS is sent */
ERA_WRITE_SMS() {
    if (!isGsmConnected) {
        return false;
    }
    ERA_LOG("ERa", "Write SMS to %s: %s", to, message);
    modem.sendSMS(to, message);
    return true;
}

/* This function is triggered whenever the board information is sent */
ERA_INFO() {
    if (!isGsmConnected || WiFi.isConnected()) {
        return;
    }
    int16_t signal = modem.getSignalQuality();

    ERaSetString(root, INFO_NETWORK_PROTOCOL, "GSM");
    ERaSetString(root, INFO_SSID, apn);
    ERaSetNumber(root, INFO_RSSI, signal);
    ERaSetNumber(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    ERaSetString(root, INFO_MAC, "GSM");
    ERaSetString(root, INFO_LOCAL_IP, "GSM");
}

/* This function is triggered whenever the Modbus information is sent */
ERA_MODBUS_INFO() {
    if (!isGsmConnected || WiFi.isConnected()) {
        return;
    }
    int16_t signal = modem.getSignalQuality();

    ERaSetNumber(root, INFO_MB_RSSI, signal);
    ERaSetNumber(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
    ERaSetString(root, INFO_MB_WIFI_USING, apn);
}

#endif /* INC_ERA_WIFI_GSM_HPP_ */
