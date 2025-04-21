#ifndef INC_ERA_UDP_HPP_
#define INC_ERA_UDP_HPP_

#include <ERa/ERaCmdHandler.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/types/WrapperString.hpp>

template <class Udp>
class ERaUdp
{
    const char* TAG = "UDP";
    const uint16_t UDP_PORT = 54321;

public:
    ERaUdp(Udp& _udp)
        : udp(_udp)
        , _connected(false)
        , stopHandle(false)
        , org(ERA_ORG_NAME)
        , model(ERA_MODEL_NAME)
        , deviceName(ERA_DEVICE_NAME)
        , deviceType(ERA_DEVICE_TYPE)
        , deviceSecret(ERA_DEVICE_SECRET)
        , dataObject(nullptr)
        , firstHandler(nullptr)
        , lastHandler(nullptr)
    {}
    ~ERaUdp()
    {
        ERaCmdHandler* handler = this->firstHandler;
        while (handler != nullptr) {
            ERaCmdHandler* next = handler->getNext();
            delete handler;
            handler = next;
        }
    }

    void begin(const char* auth = nullptr) {
        this->authToken = auth;
        this->_connected = this->udp.begin(UDP_PORT);
    }

    void stop() {
        if (!this->connected()) {
            return;
        }
        this->udp.stop();
        this->_connected = false;
    }

    void run(bool* status = nullptr) {
        if (!this->connected()) {
            return;
        }
        int size = udp.parsePacket();
        if (size) {
            char buffer[size + 1] {0};
            this->udp.read(buffer, size);
            this->parseBuffer(buffer, status);
        }
    }

    void setERaORG(const char* org) {
        this->org = org;
    }

    void setERaModel(const char* model) {
        this->model = model;
    }

    void setDeviceName(const char* name) {
        this->deviceName = name;
    }

    void setDeviceType(const char* type) {
        this->deviceType = type;
    }

    void setDeviceSecretKey(const char* key) {
        this->deviceSecret = key;
    }

    void on(const char* cmd, HandlerSimple_t fn) {
        this->addHandler(new ERaCmdHandler(cmd, fn));
    }

    bool hasArg(const char* name) {
        if (!cJSON_IsObject(this->dataObject)) {
            return this->matchArg(this->dataObject, name);
        }
        return cJSON_HasObjectItem(this->dataObject, name);
    }

    ERaString arg(const char* name) {
        if (cJSON_IsString(this->dataObject) &&
            this->matchArg(this->dataObject, name)) {
            return this->dataObject->valuestring;
        }
        else if (cJSON_IsNumber(this->dataObject) &&
            this->matchArg(this->dataObject, name)) {
            return this->dataObject->valuedouble;
        }
        else if (cJSON_IsBool(this->dataObject) &&
            this->matchArg(this->dataObject, name)) {
            return this->dataObject->valueint;
        }
        else if (!cJSON_IsObject(this->dataObject)) {
            return "";
        }

        cJSON* item = cJSON_GetObjectItem(this->dataObject, name);
        if (cJSON_IsString(item)) {
            return item->valuestring;
        }
        else if (cJSON_IsNumber(item)) {
            return item->valuedouble;
        }
        else if (cJSON_IsBool(item)) {
            return item->valueint;
        }
        return "";
    }

    void sendAndStop(const char* message) {
        this->send(message);
        this->stopHandle = true;
    }

    void send(const char* message, size_t retry = 2) {
        if (!this->connected()) {
            return;
        }
        if (!retry) {
            return;
        }

        do {
            if (this->udp.beginPacket(this->udp.remoteIP(), this->udp.remotePort())) {
                this->udp.write(reinterpret_cast<const uint8_t*>(message), strlen(message));
                if (this->udp.endPacket()) {
                #if defined(ERA_DEBUG_UDP)
                    ERA_LOG_DEBUG(TAG, ERA_PSTR("UDP >> %s"), message);
                #endif
                    break;
                }
            }
            ERA_LOG_ERROR(TAG, ERA_PSTR("Send UDP failed!"));
            ERaDelay(1000);
        } while (--retry);
    }

    void send(const char* message, size_t retry,
            unsigned long timeout) {
        if (!timeout) {
            return this->send(message, retry);
        }

        bool status {false};
        MillisTime_t started = ERaMillis();
        do {
            this->send(message, retry);
            ERaWatchdogFeed();
            MillisTime_t startMillis = ERaMillis();
            while (ERaRemainingTime(startMillis, 5000UL)) {
                this->run(&status);
                if (status) {
                    return;
                }
                ERaDelay(10);
            }
        } while (ERaRemainingTime(started, timeout));
    }

    void sendBoardInfo(unsigned long timeout = 0UL);

protected:
private:
    void parseBuffer(const char* ptr, bool* status = nullptr);
    void runCommand(const char* cmd);
    void runCommand(const char* cmd, const ERaCmdHandler* handler);
    bool matchArg(const cJSON* item, const char* name);

    ERaString getChipID();
    template <int size>
    void getWiFiName(char(&ptr)[size], bool withPrefix = true);
    template <int size>
    void getImeiChip(char(&ptr)[size]);

    void addHandler(ERaCmdHandler* handler) {
        if (this->lastHandler == nullptr) {
            this->firstHandler = handler;
        }
        else {
            this->lastHandler->setNext(handler);
        }
        this->lastHandler = handler;
    }

    bool connected() {
        return this->_connected;
    }

    Udp& udp;
    bool _connected;
    bool stopHandle;
    ERaString authToken;
    ERaString org;
    ERaString model;
    ERaString deviceName;
    ERaString deviceType;
    ERaString deviceSecret;
    cJSON* dataObject;
    ERaCmdHandler* firstHandler;
    ERaCmdHandler* lastHandler;
};

template <class Udp>
void ERaUdp<Udp>::parseBuffer(const char* ptr, bool* status) {
#if defined(ERA_DEBUG_UDP)
    ERA_LOG_DEBUG(TAG, ERA_PSTR("UDP << %s"), ptr);
#endif
    cJSON* root = cJSON_Parse(ptr);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }
    char* type = nullptr;
    cJSON* item = cJSON_GetObjectItem(root, "type");
    if (cJSON_IsString(item)) {
        type = item->valuestring;
    }
    item = cJSON_GetObjectItem(root, "data");
    if (cJSON_IsObject(item) && (type != nullptr)) {
        cJSON* current = nullptr;
        for (current = item->child; current != nullptr && current->string != nullptr; current = current->next) {
            ERaString cmd(type);
            cmd += "/";
            cmd += current->string;
            this->dataObject = current;
            this->runCommand(cmd.c_str());
            if (this->stopHandle) {
                this->stopHandle = false;
                break;
            }
        }
        current = nullptr;
    }
    if (status != nullptr) {
        item = cJSON_GetObjectItem(root, "status");
        if (cJSON_IsBool(item) || cJSON_IsNumber(item)) {
            *status = item->valueint;
        }
        else if (cJSON_IsString(item)) {
            *status = ERaStrCmp(item->valuestring, "ok");
        }
    }
    cJSON_Delete(root);
    this->dataObject = nullptr;
    root = nullptr;
    item = nullptr;
}

template <class Udp>
void ERaUdp<Udp>::runCommand(const char* cmd) {
    if (cmd == nullptr) {
        return;
    }
    ERaCmdHandler* handler = this->firstHandler;
    while (handler != nullptr) {
        ERaCmdHandler* next = handler->getNext();
        this->runCommand(cmd, handler);
        handler = next;
    }
}

template <class Udp>
void ERaUdp<Udp>::runCommand(const char* cmd, const ERaCmdHandler* handler) {
    if ((handler->cmd == nullptr) || (handler->fn == nullptr)) {
        return;
    }
    if (ERaStrCmp(handler->cmd, cmd)) {
        handler->fn();
    }
}

template <class Udp>
void ERaUdp<Udp>::sendBoardInfo(unsigned long timeout) {
    ERA_LOG(TAG, ERA_PSTR("Board Info..."));
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON_AddStringToObject(root, "type", "data_gateway");
    cJSON* item = cJSON_CreateObject();
    if (item != nullptr) {
        char imei[64] {0};
        char ssidAP[64] {0};
        this->getWiFiName(ssidAP);
        this->getImeiChip(imei);
        cJSON_AddStringToObject(item, INFO_BOARD, ERA_BOARD_TYPE);
        cJSON_AddStringToObject(item, INFO_MODEL, ERA_MODEL_TYPE);
        cJSON_AddStringToObject(item, INFO_IMEI, imei);
        cJSON_AddStringToObject(item, INFO_AUTH_TOKEN, this->authToken.c_str());
        cJSON_AddStringToObject(item, INFO_DEVICE_NAME, this->deviceName.c_str());
        cJSON_AddStringToObject(item, INFO_DEVICE_TYPE, this->deviceType.c_str());
        cJSON_AddStringToObject(item, INFO_DEVICE_SECRET, this->deviceSecret.c_str());
        cJSON_AddStringToObject(item, INFO_VERSION, ERA_VERSION);
        cJSON_AddStringToObject(item, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
        cJSON_AddStringToObject(item, INFO_SSID, ssidAP);
        cJSON_AddStringToObject(item, INFO_PASS, WIFI_AP_PASS);
#if defined(ESP32) ||   \
    defined(ESP8266)
        cJSON_AddStringToObject(item, INFO_BSSID, WiFi.softAPmacAddress().c_str());
        cJSON_AddStringToObject(item, INFO_MAC, WiFi.macAddress().c_str());
        cJSON_AddStringToObject(item, INFO_LOCAL_IP, WiFi.softAPIP().toString().c_str());
#else
        cJSON_AddStringToObject(item, INFO_BSSID, ::getBSSID().c_str());
        cJSON_AddStringToObject(item, INFO_MAC, ::getMAC().c_str());
        cJSON_AddStringToObject(item, INFO_LOCAL_IP, ::getSoftAPIP().c_str());
#endif
    }
    cJSON_AddItemToObject(root, "gateway", item);
    char* ptr = cJSON_PrintUnformatted(root);
    if (ptr != nullptr) {
        this->send(ptr, 2, timeout);
        free(ptr);
    }
    cJSON_Delete(root);
    root = nullptr;
    ptr = nullptr;
}

template <class Udp>
bool ERaUdp<Udp>::matchArg(const cJSON* item, const char* name) {
    if ((item == nullptr) || (item->string == nullptr) || (name == nullptr)) {
        return false;
    }

    return ERaStrCmp(item->string, name);
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", this->org.c_str(), this->model.c_str(),
                                      this->getChipID().c_str());
    }
    else {
        FormatString(ptr, "%s.%s", this->org.c_str(), this->getChipID().c_str());
    }
    ERaToLowerCase(ptr);
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getImeiChip(char(&ptr)[size]) {
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if (this->authToken.length()) {
        FormatString(ptr, this->authToken.c_str());
    }
    else {
        FormatString(ptr, "ERA-%s", this->getChipID().c_str());
    }
#endif
}

#endif /* INC_ERA_UDP_HPP_ */
