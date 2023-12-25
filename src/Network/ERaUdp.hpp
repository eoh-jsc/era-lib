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
        , authToken(nullptr)
        , pORG(ERA_ORG_NAME)
        , pModel(ERA_MODEL_NAME)
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
        if (org == nullptr) {
            return;
        }
        this->pORG = org;
    }

    void setERaModel(const char* model) {
        if (model == nullptr) {
            return;
        }
        this->pModel = model;
    }

    void on(const char* cmd, HandlerSimple_t fn) {
        this->addHandler(new ERaCmdHandler(cmd, fn));
    }

    ERaString arg(const char* name) {
        if (cJSON_IsString(this->dataObject)) {
            return this->dataObject->valuestring;
        }
        else if (cJSON_IsNumber(this->dataObject)) {
            return this->dataObject->valuedouble;
        }
        else if (cJSON_IsBool(this->dataObject)) {
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
    const char* authToken;
    const char* pORG;
    const char* pModel;
    cJSON* dataObject;
    ERaCmdHandler* firstHandler;
    ERaCmdHandler* lastHandler;
};

template <class Udp>
void ERaUdp<Udp>::parseBuffer(const char* ptr, bool* status) {
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
        cJSON_AddStringToObject(item, INFO_AUTH_TOKEN, this->authToken);
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
        cJSON_AddStringToObject(root, INFO_BSSID, ::getBSSID().c_str());
        cJSON_AddStringToObject(root, INFO_MAC, ::getMAC().c_str());
        cJSON_AddStringToObject(root, INFO_LOCAL_IP, ::getSoftAPIP().c_str());
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

#endif /* INC_ERA_UDP_HPP_ */
