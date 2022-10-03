#ifndef INC_MVP_UDP_HPP_
#define INC_MVP_UDP_HPP_

#include <MVP/MVPCmdHandler.hpp>
#include <Utility/MVPcJSON.hpp>
#include <Utility/MVPUtility.hpp>

template <class Udp>
class MVPUdp
{
    const char* TAG = "UDP";
    const uint16_t UDP_PORT = 54321;

public:
    MVPUdp(Udp& _udp)
        : udp(_udp)
        , _connected(false)
        , authToken(nullptr)
        , dataObject(nullptr)
        , firstHandler(nullptr)
        , lastHandler(nullptr)
    {}
    ~MVPUdp()
    {
        MVPCmdHandler* handler = this->firstHandler;
        while (handler != nullptr) {
            MVPCmdHandler* next = handler->getNext();
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

    void run() {
        if (!this->connected()) {
            return;
        }
        int size = udp.parsePacket();
        if (size) {
            char buffer[size + 1] {0};
            this->udp.read(buffer, size);
            this->parseBuffer(buffer);
        }
    }

    void on(const char* cmd, HandlerSimple_t fn) {
        this->addHandler(new MVPCmdHandler(cmd, fn));
    }

    std::string arg(const char* name) {
        if (!cJSON_IsObject(this->dataObject)) {
            return "";
        }
        cJSON* item = cJSON_GetObjectItem(this->dataObject, name);
        if (cJSON_IsString(item)) {
            return item->valuestring;
        }
        else if (cJSON_IsNumber(item)) {
            return std::to_string(item->valuedouble);
        }
        else if (cJSON_IsBool(item)) {
            return std::to_string(item->valueint);
        }
        return "";
    }

    void send(const char* message) {
        if (!this->connected()) {
            return;
        }
        this->udp.beginPacket(this->udp.remoteIP(), this->udp.remotePort());
        this->udp.write(reinterpret_cast<const uint8_t*>(message), strlen(message));
        this->udp.endPacket();
    }

    void sendBoardInfo();

protected:
private:
    void parseBuffer(const char* ptr);
    void runCommand(const char* cmd);
    void runCommand(const char* cmd, MVPCmdHandler* handler);
    void getWiFiName(char* ptr, size_t size, bool withPrefix = true);
    void getImeiChip(char* ptr, size_t size);

    void addHandler(MVPCmdHandler* handler) {
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
    cJSON* dataObject;
    MVPCmdHandler* firstHandler;
    MVPCmdHandler* lastHandler;
};

template <class Udp>
void MVPUdp<Udp>::parseBuffer(const char* ptr) {
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
    if (cJSON_IsObject(item) && type != nullptr) {
        for (cJSON* current = item->child; current != nullptr && current->string != nullptr; current = current->next) {
            std::string cmd(type);
            cmd.append("/");
            cmd.append(current->string);
            this->dataObject = current;
            this->runCommand(cmd.c_str());
        }
    }
    cJSON_Delete(root);
    this->dataObject = nullptr;
    root = nullptr;
    item = nullptr;
}

template <class Udp>
void MVPUdp<Udp>::runCommand(const char* cmd) {
    if (cmd == nullptr) {
        return;
    }
    MVPCmdHandler* handler = this->firstHandler;
    while (handler != nullptr) {
        MVPCmdHandler* next = handler->getNext();
        this->runCommand(cmd, handler);
        handler = next;
    }
}

template <class Udp>
void MVPUdp<Udp>::runCommand(const char* cmd, MVPCmdHandler* handler) {
    if (handler->cmd == nullptr || handler->fn == nullptr) {
        return;
    }
    if (MVPStrCmp(handler->cmd, cmd)) {
        handler->fn();
    }
}

template <class Udp>
void MVPUdp<Udp>::sendBoardInfo() {
    MVP_LOG(TAG, "Board Info...");
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON_AddStringToObject(root, "type", "data_gateway");
    cJSON* item = cJSON_CreateObject();
    if (item != nullptr) {
        char imei[64] {0};
        char ssidAP[64] {0};
        this->getWiFiName(ssidAP, sizeof(ssidAP));
        this->getImeiChip(imei, sizeof(imei));
        cJSON_AddStringToObject(item, "board", MVP_BOARD_TYPE);
        cJSON_AddStringToObject(item, "model", MVP_MODEL_TYPE);
        cJSON_AddStringToObject(item, "imei", imei);
        cJSON_AddStringToObject(item, "firmware_version", MVP_FIRMWARE_VERSION);
        cJSON_AddStringToObject(item, "ssid", ssidAP);
        cJSON_AddStringToObject(item, "pass", WIFI_AP_PASS);
        cJSON_AddStringToObject(item, "bssid", WiFi.softAPmacAddress().c_str());
        cJSON_AddStringToObject(item, "mac", WiFi.macAddress().c_str());
        cJSON_AddStringToObject(item, "ip", WiFi.softAPIP().toString().c_str());
    }
    cJSON_AddItemToObject(root, "gateway", item);
    char* ptr = cJSON_PrintUnformatted(root);
    if (ptr != nullptr) {
        this->send(ptr);
        free(ptr);
    }
    cJSON_Delete(root);
    root = nullptr;
    ptr = nullptr;
}

template <class Udp>
void MVPUdp<Udp>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    if (withPrefix) {
        snprintf(ptr, size, "eoh.mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    } else {
        snprintf(ptr, size, "mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
}

template <class Udp>
void MVPUdp<Udp>::getImeiChip(char* ptr, size_t size) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
#ifdef MVP_AUTH_TOKEN
    snprintf(ptr, size, MVP_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "MVP-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
#endif
}

#endif