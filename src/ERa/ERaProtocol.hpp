#ifndef INC_ERA_PROTOCOL_HPP_
#define INC_ERA_PROTOCOL_HPP_

#if defined(__has_include)
    #if __has_include("ERaOptions.hpp")
        #include "ERaOptions.hpp"
    #endif
#endif

#if defined(ARDUINO_ARCH_SAM)
    #undef PWM
#endif

#include <Storage/ERaFlashDef.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaProtocolDef.hpp>
#include <ERa/ERaHandlers.hpp>
#include <ERa/ERaState.hpp>
#include <ERa/ERaApi.hpp>
#include <ERa/ERaModel.hpp>
#include <ERa/ERaTask.hpp>
#include <ERa/ERaTranspHandler.hpp>
#include <ERa/ERaLogger.hpp>
#include <ERa/ERaCallbacks.hpp>
#include <OTA/ERaOTA.hpp>
#include <Utility/ERaUUID.hpp>
#include <Utility/ERaInfo.hpp>

template <class Transp, class Flash>
class ERaProto
    : public ERaApi< ERaProto<Transp, Flash>, Flash >
#if defined(ERA_OTA)
    , public ERaOTA< ERaProto<Transp, Flash>, Flash >
#endif
    , public ERaModel
    , public ERaTranspHandler
    , public ERaServerCallbacks
{
    enum ERaChipCfgT {
        CHIP_UPDATE_CONFIG = 0,
        CHIP_UPDATE_CONTROL = 1,
        CHIP_UPDATE_BLUETOOTH = 2,
        CHIP_SCAN_MODBUS = 3,
        CHIP_SEND_SMS = 4,
        CHIP_CONTROL_ALIAS = 5,
        CHIP_IO_PIN = 6,
        CHIP_OTA = 7
    };
    typedef void* ApiData_t;
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> StateCallback_t;
    typedef std::function<void(void)> FunctionCallback_t;
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*StateCallback_t)(void);
    typedef void (*FunctionCallback_t)(void);
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

    char ERA_TOPIC[MAX_TOPIC_LENGTH];
    const char* TAG = "Protocol";
    const char* BASE_TOPIC = ERA_MQTT_BASE_TOPIC;
    friend class ERaApi< ERaProto<Transp, Flash>, Flash >;
    typedef ERaApi< ERaProto<Transp, Flash>, Flash > Base;
#if defined(ERA_OTA)
    friend class ERaOTA< ERaProto<Transp, Flash>, Flash >;
    typedef ERaOTA< ERaProto<Transp, Flash>, Flash > OTA;
#endif
    typedef ERaTranspHandler Handler;

public:
    ERaProto(Transp& _transp, Flash& _flash)
        : Base(_flash)
#if defined(ERA_OTA)
        , OTA(_flash)
#endif
        , transp(_transp)
        , pLogger(nullptr)
        , pServerCallbacks(nullptr)
        , nonBlocking(false)
        , _connected(false)
        , heartbeat(ERA_HEARTBEAT_INTERVAL * 12UL)
        , lastHeartbeat(0UL)
        , selfInfo(ERA_SELF_INFO_INTERVAL * 10UL)
        , lastSelfInfo(0UL)
    {
        memset(this->ERA_TOPIC, 0, sizeof(this->ERA_TOPIC));
    }
    ~ERaProto()
    {}

    void begin(const char* auth) {
        ClearArray(this->ERA_TOPIC);
        FormatString(this->ERA_TOPIC, "%s/%s", BASE_TOPIC, auth);
        this->transp.setAuth(auth);
        this->transp.setTopic(this->ERA_TOPIC);
        this->transp.onAppLoop(Base::appCb);
        this->transp.onMessage(this->messageCb);
        this->transp.onStateChange(this->connectedCb,
                                   this->disconnectedCb);
#if defined(ERA_AUTOMATION)
        Base::initAutomation(auth, this->ERA_TOPIC, this->messageCb);
#endif
        Base::initComponent(auth, this->ERA_TOPIC, this->messageCb);
        Handler::setAuth(auth);
        Handler::setTopic(this->ERA_TOPIC);
        Handler::onMessage(this->messageCb);
        Handler::onStateChange(this->connectedCb,
                               this->disconnectedCb);
        Base::postBegin();
    }

    void run() {
        if (!Base::run(false) && !this->isConfigMode()) {
            ERaState::set(StateT::STATE_DISCONNECTED);
        }
        if (Base::isNeedSyncConfig()) {
            this->syncConfig();
        }
        this->publishHeartbeat();
        this->publishSelfInfo();
    }

    void sendCommand(const char* auth, ERaRsp_t& rsp, ApiData_t data = nullptr);

    void setServerCallbacks(ERaServerCallbacks& rCallbacks) override {
        this->pServerCallbacks = &rCallbacks;
    }

    void setServerCallbacks(ERaServerCallbacks* pCallbacks) override {
        this->pServerCallbacks = pCallbacks;
    }

    void setERaTransp(ERaTransp& rTransp) override {
        Handler::addTransp(rTransp);
    }

    void setERaTransp(ERaTransp* pTransp) override {
        Handler::addTransp(pTransp);
    }

    void setERaLogger(ERaLogger& rLog) override {
        this->pLogger = &rLog;
    }

    void setERaLogger(ERaLogger* pLog) override {
        this->pLogger = pLog;
    }

    ERaLogger* getERaLogger() const {
        return this->pLogger;
    }

    void setRootCA(const char* ca) {
        this->transp.setRootCA(ca);
    }

    void setBoardID(const char* boardID) {
        this->transp.setClientID(boardID);
    }

    const char* getBoardID() const {
        return this->transp.getClientID();
    }

    void setTimeout(uint32_t timeout) {
        this->transp.setTimeout(timeout);
    }

    void setSkipACK(bool skip = true) {
        this->transp.setSkipACK(skip);
    }

    void setKeepAlive(uint16_t keepAlive) {
        if (keepAlive > 1000UL) {
            keepAlive /= 1000UL;
        }
        this->transp.setKeepAlive(keepAlive);
    }

    void setDropOverflow(bool enabled = false) {
        this->transp.dropOverflow(enabled);
    }

    void setNonBlocking(bool enable) {
        this->nonBlocking = enable;
        this->transp.setNonBlocking(enable);
        if (enable) {
            Base::setAppLoop(enable);
        }
    }

    void setHeartbeat(unsigned long interval) {
        if (!interval) {
        }
        else if (interval < ERA_HEARTBEAT_INTERVAL) {
            interval = ERA_HEARTBEAT_INTERVAL;
        }
        this->heartbeat = interval;
    }

    void setSelfInfo(unsigned long interval) {
        if (!interval) {
        }
        else if (interval < ERA_SELF_INFO_INTERVAL) {
            interval = ERA_SELF_INFO_INTERVAL;
        }
        this->selfInfo = interval;
    }

    const char* getAuth() const {
        return this->transp.getAuth();
    }

    void syncConfig() {
        this->transp.syncConfig();
    }

    ERA_DEPRECATED
    void askConfigWhenRestart(bool enable = true) {
        this->transp.setAskConfig(enable);
    }

    Transp& getTransp() const {
        return this->transp;
    }

    void disconnect() {
        this->transp.disconnect();
        this->onDisconnected();
    }

    bool connected() override {
        return (this->_connected && this->transp.connected());
    }

protected:
    void init() {
        Base::begin();
    }

    bool connect(FunctionCallback_t fn = nullptr) {
        if (!this->transp.connect(fn)) {
            return false;
        }
        this->printBanner();
        this->sendInfo();
        return true;
    }

    void runEdge() {
        Handler::run();
    }

    virtual void requestListWiFi() override;
    virtual void responseListWiFi() override;
    virtual void connectNewWiFi(const char* ssid, const char* pass) override;
    virtual void connectNewNetworkResult() override;

    bool sendListWiFi(const char* payload);
    bool sendChangeResultWiFi(const char* payload);
    bool publishData(const char* prefixTopic, const char* payload,
                            bool retained, bool extended = false);
    bool clearRetainedData(const char* prefixTopic);
    bool isConfigMode() const;
    bool isNonBlocking() const;
    void onConnected();
    void onDisconnected();

private:
    void printBanner();
    void processDownRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processDownAction(const char* payload, cJSON* root, cJSON* item);
    bool processActionChip(const char* payload, cJSON* root, uint8_t type);
    void processDownCommand(const char* payload, cJSON* root, cJSON* item);
    void processFinalize(const char* payload, cJSON* root);
    void processConfiguration(const char* payload, const char* hash, cJSON* root);
    void processArduinoPinConfig(cJSON* root);
    void processDeviceConfig(cJSON* root, uint8_t type);
    void processIOPin(cJSON* root);
    void processPinRequest(const ERaDataBuff& arrayTopic, const char* payload, size_t index);
    void processWiFiRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processChangeWiFi(const char* payload);
#if defined(ERA_AUTOMATION)
    void processAutomationRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processAutomationCommand(const char* payload, cJSON* root, cJSON* item);
    void processAutomationFinalize(const char* payload, cJSON* root);
    void processAutomationConfiguration(const char* payload, const char* hash, cJSON* root);
#endif
#if defined(ERA_ZIGBEE)
    void processZigbeeRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processDeviceZigbee(const char* payload);
    void processActionDeviceZigbee(const cJSON* const root, uint8_t type);
    void processActionZigbee(const char* ieeeAddr, const char* payload, uint8_t type);
#endif
    void processState(const ERaDataBuff& arrayTopic, const char* payload);
    void publishHeartbeat();
    void publishSelfInfo();
    bool sendInfo();
    bool sendSelfInfo();
    bool sendData(ERaRsp_t& rsp);
    bool sendPinData(ERaRsp_t& rsp);
    bool sendPinMultiData(ERaRsp_t& rsp);
    bool sendConfigIdData(ERaRsp_t& rsp);
    bool sendConfigIdLogger(ERaRsp_t& rsp, bool status,
                            char* buf, size_t len);
    bool sendConfigIdMultiData(ERaRsp_t& rsp);
    bool sendNotifyData(ERaRsp_t& rsp);
    bool sendEmailData(ERaRsp_t& rsp);
    bool sendWebhookData(ERaRsp_t& rsp);
    bool sendSelfData(ERaRsp_t& rsp);
#if defined(ERA_AUTOMATION)
    bool sendAutomation(ERaRsp_t& rsp);
#endif
#if defined(ERA_MODBUS)
    bool sendModbusData(ERaRsp_t& rsp);
    void clearRetainedModbusData();
#endif
#if defined(ERA_ZIGBEE)
    bool sendZigbeeData(ERaRsp_t& rsp);
    void clearRetainedZigbeeData(const char* id);
#endif
#if defined(ERA_SPECIFIC)
    bool sendSpecificData(ERaRsp_t& rsp);
#endif
    void sendCommandVirtualMulti(const char* auth, ERaRsp_t& rsp, ERaDataJson* data);
#if defined(ERA_ZIGBEE)
    void sendCommandZigbee(const char* auth, ERaRsp_t& rsp);
#endif
    void sendCommand(ERaRsp_t& rsp, ApiData_t data = nullptr);
    void sendCommandVirtual(ERaRsp_t& rsp, ERaDataJson* data);
#if defined(ERA_MODBUS)
    void sendCommandModbus(ERaRsp_t& rsp, ERaDataBuff* data);
#endif
    void clearRetainedConfigIdData(ERaInt_t configId);

    void processRequest(const char* topic, const char* payload);
    void processExternalRequest(const ERaDataBuff& topic, const char* payload);
#if !defined(ERA_HAS_FUNCTIONAL_H)
    static void _onConnected();
    static void _onDisconnected();
    static void _processRequest(const char* topic, const char* payload);
#endif

    size_t splitString(char* strInput, const char* delims);

    void onWrite(const char* topic, const char* payload) override {
        this->processRequest(topic, payload);
    }

#if defined(ERA_HAS_FUNCTIONAL_H)
    StateCallback_t connectedCb = [&, this](void) {
        this->onConnected();
    };
    StateCallback_t disconnectedCb = [&, this](void) {
        this->onDisconnected();
    };
    MessageCallback_t messageCb = [&, this](const char* topic, const char* payload) {
        this->processRequest(topic, payload);
    };
#else
    StateCallback_t connectedCb = [](void) {
        ERaProto::_onConnected();
    };
    StateCallback_t disconnectedCb = [](void) {
        ERaProto::_onDisconnected();
    };
    MessageCallback_t messageCb = [](const char* topic, const char* payload) {
        ERaProto::_processRequest(topic, payload);
    };
#endif

    Transp& transp;
    ERaLogger* pLogger;
    ERaServerCallbacks* pServerCallbacks;
    bool nonBlocking;
    bool _connected;

    unsigned long heartbeat;
    unsigned long lastHeartbeat;
    unsigned long selfInfo;
    unsigned long lastSelfInfo;
};

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::printBanner() {
    ERA_LOG(TAG, ERA_PSTR(ERA_NEWLINE
        "  ____  ____              " ERA_NEWLINE
        " / _   / _  \\  _         " ERA_NEWLINE
        "/____ / / __/ /.\\        " ERA_NEWLINE
        " / _ /  _ \\  / _ \\      " ERA_NEWLINE
        "/___/__//_/`/_/ \\_\\     " ERA_NEWLINE
        "           (v" ERA_VERSION " for " ERA_BOARD_TYPE ")"
        ERA_NEWLINE
        "Connection successful (ping: %lums)!!!" ERA_NEWLINE),
        (unsigned long)this->transp.getPing()
    );
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::isConfigMode() const {
    switch (ERaState::get()) {
        case StateT::STATE_SWITCH_TO_AP:
        case StateT::STATE_RESET_CONFIG:
        case StateT::STATE_RESET_CONFIG_REBOOT:
        case StateT::STATE_REBOOT:
            return true;
        default:
            return false;
    }
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::isNonBlocking() const {
    return this->nonBlocking;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::onConnected() {
    if (this->_connected) {
        return;
    }

    this->_connected = true;
    ERaOnConnected();
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::onDisconnected() {
    if (!this->_connected) {
        return;
    }

    this->_connected = false;
    this->transp.setConnected(false);
    Base::switchTask();
    ERaOnDisconnected();
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processRequest(const char* topic, const char* payload) {
    if (payload == nullptr) {
        return;
    }

    ERA_LOG(this->transp.getTag(), ERA_PSTR("Message %s (% 3d): %s"), topic, strlen(payload), payload);

    char copy[MAX_TOPIC_LENGTH] {0};
    CopyToString(ERaFindStr(topic, BASE_TOPIC) + strlen(BASE_TOPIC) + 1, copy);
    ERaDataBuff arrayTopic(copy, strlen(copy) + 1, MAX_TOPIC_LENGTH);
    this->splitString(copy, "/");

    if (ERaFindStr(topic, this->ERA_TOPIC) != topic) {
        this->processExternalRequest(arrayTopic, payload);
        return;
    }

    if (arrayTopic.size() < 2) {
        this->processExternalRequest(arrayTopic, payload);
        return;
    }

    if (arrayTopic.at(1) == "arduino_pin") {
        Base::processArduinoPinRequest(arrayTopic, payload);
    }
    else if (arrayTopic.at(1) == "virtual_pin") {
        Base::processVirtualPinRequest(arrayTopic, payload);
    }
    else if (arrayTopic.at(1) == "down") {
        this->processDownRequest(arrayTopic, payload);
    }
#if defined(ERA_AUTOMATION)
    else if (arrayTopic.at(1) == "local_control") {
        this->processAutomationRequest(arrayTopic, payload);
    }
#endif
    else if (arrayTopic.at(1) == "wifi") {
        this->processWiFiRequest(arrayTopic, payload);
    }
    else if (arrayTopic.at(1) == "is_online") {
        this->processState(arrayTopic, payload);
    }
    else if (arrayTopic.at(1) == "pin") {
        this->processPinRequest(arrayTopic, payload, 2);
    }
#if defined(ERA_ZIGBEE)
    else if (arrayTopic.at(1) == "zigbee") {
        this->processZigbeeRequest(arrayTopic, payload);
    }
#endif

    this->processExternalRequest(arrayTopic, payload);

#if defined(ERA_DEBUG_PREFIX)
    const char* debug = ERA_DEBUG_PREFIX;
    if (strchr(debug, '/') != debug) {
        return;
    }
    if (arrayTopic.size() < 3) {
        return;
    }
    if (arrayTopic.at(2) == "pin") {
        this->processPinRequest(arrayTopic, payload, 3);
    }
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processExternalRequest(const ERaDataBuff& topic, const char* payload) {
    if (this->pServerCallbacks != nullptr) {
        this->pServerCallbacks->onWrite(topic, payload);
    }
    Base::messageComponent(topic, payload);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDownRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    cJSON* root = cJSON_Parse(payload);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "action");
    if (cJSON_IsString(item)) {
        this->processDownAction(payload, root, item);
    }
    item = cJSON_GetObjectItem(root, "command");
    if (cJSON_IsString(item)) {
        this->processDownCommand(payload, root, item);
    }

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
    ERA_FORCE_UNUSED(arrayTopic);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDownAction(const char* payload, cJSON* root, cJSON* item) {
    if (ERaStrCmp(item->valuestring, "update_firmware")) {
#if defined(ERA_OTA)
        if (!this->processActionChip(payload, root, ERaChipCfgT::CHIP_OTA)) {
            OTA::begin();
        }
#endif
        ERaState::set(StateT::STATE_OTA_UPGRADE);
    }
    else if (ERaStrCmp(item->valuestring, "reset_eeprom")) {
        Base::eraseAllConfigs();
        ERaDelay(1000);
        ERaState::set(StateT::STATE_RESET_CONFIG_REBOOT);
    }
    else if (ERaStrCmp(item->valuestring, "force_reset")) {
        ERaRestart(false);
    }
    else if (ERaStrCmp(item->valuestring, "send_sms")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_SEND_SMS);
    }
#if defined(ERA_MODBUS)
    else if (ERaStrCmp(item->valuestring, "update_configuration")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_UPDATE_CONFIG);
    }
    else if (ERaStrCmp(item->valuestring, "send_control")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_UPDATE_CONTROL);
    }
    else if (ERaStrCmp(item->valuestring, "send_command")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_CONTROL_ALIAS);
    }
    else if (ERaStrCmp(item->valuestring, "scan_modbus")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_SCAN_MODBUS);
    }
#endif
#if defined(ERA_BT)
    else if (ERaStrCmp(item->valuestring, "update_bluetooth")) {
        this->processActionChip(payload, root, ERaChipCfgT::CHIP_UPDATE_BLUETOOTH);
    }
#endif

    ERA_FORCE_UNUSED(root);
    ERA_FORCE_UNUSED(item);
    ERA_FORCE_UNUSED(payload);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::processActionChip(const char* payload, cJSON* root, uint8_t type) {
    cJSON* dataItem = cJSON_GetObjectItem(root, "data");
    if (!cJSON_IsObject(dataItem)) {
        return false;
    }

    cJSON* item = nullptr;
    const char* hash = nullptr;
    switch (type) {
        case ERaChipCfgT::CHIP_SEND_SMS: {
            item = cJSON_GetObjectItem(dataItem, "messages");
            if (!cJSON_IsArray(item)) {
                break;
            }
            cJSON* subItem = nullptr;
            for (int i = 0; i < cJSON_GetArraySize(item); ++i) {
                subItem = cJSON_GetArrayItem(item, i);
                if (!cJSON_IsObject(subItem)) {
                    continue;
                }
                cJSON* toItem = cJSON_GetObjectItem(subItem, "to");
                cJSON* messItem = cJSON_GetObjectItem(subItem, "message");
                if (!cJSON_IsString(toItem) || !cJSON_IsString(messItem)) {
                    continue;
                }
                this->sendSMS(toItem->valuestring, messItem->valuestring);
            }
        }
            break;
#if defined(ERA_OTA)
        case ERaChipCfgT::CHIP_OTA: {
            cJSON* info = nullptr;
            const char* otaType = nullptr;
            size_t downSize = ERA_OTA_BUFFER_SIZE;
            item = cJSON_GetObjectItem(dataItem, "type");
            if (cJSON_IsString(item)) {
                otaType = item->valuestring;
            }
            item = cJSON_GetObjectItem(dataItem, "hash");
            if (cJSON_IsString(item)) {
                hash = item->valuestring;
            }
            item = cJSON_GetObjectItem(dataItem, "down_size");
            if (cJSON_IsNumber(item) &&
                (item->valueint > 0)) {
                downSize = item->valueint;
            }
            item = cJSON_GetObjectItem(dataItem, "info");
            if (cJSON_IsObject(item)) {
                info = item;
            }
            item = cJSON_GetObjectItem(dataItem, "url");
            if (cJSON_IsString(item)) {
                OTA::begin(item->valuestring, hash, otaType, downSize, info);
            }
        }
            break;
#endif
#if defined(ERA_MODBUS)
        case ERaChipCfgT::CHIP_UPDATE_CONFIG:
            item = cJSON_GetObjectItem(dataItem, "hash_id");
            if (cJSON_IsString(item)) {
                hash = item->valuestring;
            }
            item = cJSON_GetObjectItem(dataItem, "configuration");
            if (cJSON_IsString(item)) {
                Base::Modbus::parseModbusConfig(item->valuestring, hash, payload, false, false);
                break;
            }
            item = cJSON_GetObjectItem(dataItem, "modbus_configuration");
            if (cJSON_IsObject(item)) {
                Base::Modbus::parseModbusConfig(item, hash, payload, false, true);
            }
            break;
        case ERaChipCfgT::CHIP_UPDATE_CONTROL:
            item = cJSON_GetObjectItem(dataItem, "hash_id");
            if (cJSON_IsString(item)) {
                hash = item->valuestring;
            }
            item = cJSON_GetObjectItem(dataItem, "control");
            if (cJSON_IsString(item)) {
                Base::Modbus::parseModbusConfig(item->valuestring, hash, payload, true, false);
                break;
            }
            item = cJSON_GetObjectItem(dataItem, "modbus_control");
            if (cJSON_IsObject(item)) {
                Base::Modbus::parseModbusConfig(item, hash, payload, true, true);
            }
            break;
        case ERaChipCfgT::CHIP_CONTROL_ALIAS: {
            uint16_t paramAction {0};
            uint8_t typeAction {ModbusActionTypeT::MODBUS_ACTION_DEFAULT};
            item = cJSON_GetObjectItem(dataItem, "value");
            if (cJSON_IsNumber(item)) {
                paramAction = item->valueint;
                typeAction = ModbusActionTypeT::MODBUS_ACTION_PARAMS;
            }
            else if (cJSON_IsBool(item)) {
                paramAction = (item->valueint ? MODBUS_SINGLE_COIL_ON :
                                                MODBUS_SINGLE_COIL_OFF);
                typeAction = ModbusActionTypeT::MODBUS_ACTION_PARAMS;
            }
            else if (cJSON_IsString(item) && strlen(item->valuestring)) {
                paramAction = atol(item->valuestring);
                typeAction = ModbusActionTypeT::MODBUS_ACTION_PARAMS;
            }
            item = cJSON_GetObjectItem(dataItem, "commands");
            if (cJSON_IsArray(item)) {
                cJSON* keyItem = nullptr;
                for (int i = 0; i < cJSON_GetArraySize(item); ++i) {
                    keyItem = cJSON_GetArrayItem(item, i);
                    if (cJSON_IsString(keyItem)) {
                        // Handle command
                        Base::Modbus::addModbusAction(keyItem->valuestring,
                                                    typeAction, paramAction);
                    }
                }
            }
        }
            break;
        case ERaChipCfgT::CHIP_SCAN_MODBUS:
            item = cJSON_GetObjectItem(dataItem, "scan_data");
            if (cJSON_IsString(item)) {
                Base::Modbus::parseModbusScan(item->valuestring);
            }
            break;
#endif
#if defined(ERA_BT)
        case ERaChipCfgT::CHIP_UPDATE_BLUETOOTH:
            item = cJSON_GetObjectItem(dataItem, "hash_id");
            if (cJSON_IsString(item)) {
                hash = item->valuestring;
            }
            item = cJSON_GetObjectItem(dataItem, "bluetooth");
            if (cJSON_IsString(item)) {
                Base::parseBluetoothConfig(item->valuestring, hash, payload);
            }
            break;
#endif
        default:
            return false;
    }

    ERA_FORCE_UNUSED(item);
    ERA_FORCE_UNUSED(hash);
    ERA_FORCE_UNUSED(dataItem);
    return true;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDownCommand(const char* payload, cJSON* root, cJSON* item) {
    if (ERaStrCmp(item->valuestring, "finalize_configuration")) {
        this->processFinalize(payload, root);
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processFinalize(const char* payload, cJSON* root) {
    const char* hash = nullptr;
    cJSON* item = cJSON_GetObjectItem(root, "hash_id");
    if (cJSON_IsString(item)) {
        hash = item->valuestring;
    }
    item = cJSON_GetObjectItem(root, "configuration");
    if (cJSON_IsObject(item)) {
        this->processConfiguration(payload, hash, item);
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processConfiguration(const char* payload, const char* hash, cJSON* root) {
    cJSON* item = cJSON_GetObjectItem(root, "arduino_pin");
    if (cJSON_IsObject(item)) {
        Base::processPinConfig(item, hash, payload);
    }
#if defined(ERA_ZIGBEE)
    item = cJSON_GetObjectItem(root, "zigbee");
    if (cJSON_IsObject(item)) {
        Base::Zigbee::processConfigMapZigbee(item, hash, payload);
    }
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processArduinoPinConfig(cJSON* root) {
    this->processDeviceConfig(root, ERaChipCfgT::CHIP_IO_PIN);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDeviceConfig(cJSON* root, uint8_t type) {
    cJSON* item = cJSON_GetObjectItem(root, "devices");
    if (!cJSON_IsArray(item)) {
        return;
    }
    cJSON* current = nullptr;

    for (current = item->child; current != nullptr; current = current->next) {
        switch (type) {
            case ERaChipCfgT::CHIP_IO_PIN:
                this->processIOPin(current);
                break;
            default:
                break;
        }
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processIOPin(cJSON* root) {
    if (!cJSON_IsObject(root)) {
        return;
    }
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
        if (ERaStrCmp(current->string, "read_pins")) {
            Base::handleReadPin(current);
        }
        if (ERaStrCmp(current->string, "control_pins")) {
            Base::handleWritePin(current);
        }
        if (ERaStrCmp(current->string, "virtual_pins")) {
            Base::handleVirtualPin(current);
        }
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processPinRequest(const ERaDataBuff& arrayTopic, const char* payload, size_t index) {
    if (arrayTopic.size() <= index) {
        return;
    }

    if (arrayTopic.at(index) == "down") {
        Base::handlePinRequest(arrayTopic, payload);
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processWiFiRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    if (arrayTopic.size() != 3) {
        return;
    }

    if (arrayTopic.at(2) == "ask") {
        this->requestListWiFi();
    }
    else if (arrayTopic.at(2) == "change") {
        this->processChangeWiFi(payload);
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processChangeWiFi(const char* payload) {
    cJSON* root = cJSON_Parse(payload);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    const char* ssid = nullptr;
    const char* password = nullptr;
    cJSON* item = cJSON_GetObjectItem(root, "ssid");
    if (cJSON_IsString(item)) {
        ssid = item->valuestring;
    }
    item = cJSON_GetObjectItem(root, "password");
    if (cJSON_IsString(item)) {
        password = item->valuestring;
    }

    this->connectNewWiFi(ssid, password);

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
}

#if defined(ERA_AUTOMATION)
    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processAutomationRequest(const ERaDataBuff& arrayTopic, const char* payload) {
        cJSON* root = cJSON_Parse(payload);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }

        cJSON* item = cJSON_GetObjectItem(root, "command");
        if (cJSON_IsString(item)) {
            this->processAutomationCommand(payload, root, item);
        }

        cJSON_Delete(root);
        root = nullptr;
        item = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processAutomationCommand(const char* payload, cJSON* root, cJSON* item) {
        if (ERaStrCmp(item->valuestring, "finalize_local_control")) {
            this->processAutomationFinalize(payload, root);
        }
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processAutomationFinalize(const char* payload, cJSON* root) {
        const char* hash = nullptr;
        cJSON* item = cJSON_GetObjectItem(root, "hash_id");
        if (cJSON_IsString(item)) {
            hash = item->valuestring;
        }
        item = cJSON_GetObjectItem(root, "configuration");
        if (cJSON_IsObject(item)) {
            this->processAutomationConfiguration(payload, hash, item);
        }
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processAutomationConfiguration(const char* payload, const char* hash, cJSON* root) {
        cJSON* item = cJSON_GetObjectItem(root, "local_control");
        if (cJSON_IsArray(item)) {
            Base::processAutomationConfig(item, hash, payload);
        }
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processZigbeeRequest(const ERaDataBuff& arrayTopic, const char* payload) {
        if (arrayTopic.size() == 3) {
            if ((arrayTopic.at(2) == "permit_to_join") ||
                (arrayTopic.at(2) == "remove_device")) {
                this->processDeviceZigbee(payload);
                return;
            }
        }

        if (arrayTopic.size() < 4) {
            return;
        }

        if (arrayTopic.at(3) == "down") {
            this->processActionZigbee(arrayTopic.at(2).getString(), payload, ZigbeeActionT::ZIGBEE_ACTION_SET);
        }
        else if (arrayTopic.at(3) == "get") {
            this->processActionZigbee(arrayTopic.at(2).getString(), payload, ZigbeeActionT::ZIGBEE_ACTION_GET);
        }
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processDeviceZigbee(const char* payload) {
        cJSON* root = cJSON_Parse(payload);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }

        cJSON* item = cJSON_GetObjectItem(root, "action");
        if (item == cJSON_Invalid) {
            item = cJSON_GetObjectItem(root, "command");
        }
        if (cJSON_IsString(item)) {
            if (ERaStrCmp(item->valuestring, "search_device")) {
                this->processActionDeviceZigbee(root, ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN);
            }
            else if (ERaStrCmp(item->valuestring, "remove_zigbee")) {
                this->processActionDeviceZigbee(root, ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE);
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        item = nullptr;
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processActionDeviceZigbee(const cJSON* const root, uint8_t type) {
        cJSON* data = cJSON_GetObjectItem(root, "data");
        if (!cJSON_IsObject(root)) {
            return;
        }

        cJSON* item = nullptr;
        cJSON* payload = nullptr;
        switch (type) {
            case ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN:
                item = cJSON_GetObjectItem(data, "zigbee");
                if (cJSON_IsBool(item)) {
                    payload = cJSON_CreateObject();
                    if (payload == nullptr) {
                        break;
                    }

                    cJSON_AddBoolToObject(payload, "value", item->valueint);
                    cJSON* ieeeItem = cJSON_GetObjectItem(data, "ieee_addr");
                    if (cJSON_IsString(ieeeItem)) {
                        cJSON_AddStringToObject(payload, "ieee_addr", ieeeItem->valuestring);
                    }
                    cJSON* nwkItem = cJSON_GetObjectItem(data, "nwk_addr");
                    if (cJSON_IsString(nwkItem)) {
                        cJSON_AddStringToObject(payload, "nwk_addr", nwkItem->valuestring);
                    }
                    cJSON* codeItem = cJSON_GetObjectItem(data, "install_code");
                    if (cJSON_IsString(codeItem)) {
                        cJSON_AddStringToObject(payload, "install_code", codeItem->valuestring);
                    }

                    if (!Base::Zigbee::addZigbeeAction(ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN, "", payload)) {
                        cJSON_Delete(payload);
                    }
                    ieeeItem = nullptr;
                    nwkItem = nullptr;
                    codeItem = nullptr;
                }
                break;
            case ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE:
                item = cJSON_GetObjectItem(data, "ieee_addr");
                if (item == cJSON_Invalid) {
                    item = cJSON_GetObjectItem(data, "ieee_address");
                }
                if (cJSON_IsString(item)) {
                    this->processActionZigbee(item->valuestring, "{}", ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE);
                }
                break;
            default:
                break;
        }
        item = nullptr;
        data = nullptr;
        payload = nullptr;
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::processActionZigbee(const char* ieeeAddr, const char* payload, uint8_t type) {
        cJSON* root = cJSON_Parse(payload);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }

        if (!Base::Zigbee::addZigbeeAction(static_cast<ZigbeeActionT>(type), ieeeAddr, root)) {
            cJSON_Delete(root);
        }
        root = nullptr;
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::requestListWiFi() {
    ERaState::set(StateT::STATE_REQUEST_LIST_WIFI);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::responseListWiFi() {
    this->sendListWiFi("[]");
    ERaState::set(StateT::STATE_RUNNING);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::connectNewWiFi(const char* ssid, const char* pass) {
    ERaState::set(StateT::STATE_CONNECTING_NEW_NETWORK);
    ERA_FORCE_UNUSED(ssid);
    ERA_FORCE_UNUSED(pass);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::connectNewNetworkResult() {
    this->sendChangeResultWiFi(R"json({"success":0})json");
    ERaState::set(StateT::STATE_RUNNING);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processState(const ERaDataBuff& arrayTopic, const char* payload) {
    if (ERaStrCmp(payload, this->transp.getLWTPayload())) {
        this->transp.publishState(true);
    }
    ERA_FORCE_UNUSED(arrayTopic);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::publishHeartbeat() {
    if (!this->heartbeat) {
        return;
    }

    unsigned long currentMillis = ERaMillis();
    if ((currentMillis - this->lastHeartbeat) < this->heartbeat) {
        return;
    }
    unsigned long skipTimes = ((currentMillis - this->lastHeartbeat) / this->heartbeat);
    // update time
    this->lastHeartbeat += (this->heartbeat * skipTimes);

    if (this->connected()) {
        this->sendInfo();
    }
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::publishSelfInfo() {
    if (!this->selfInfo) {
        return;
    }

    unsigned long currentMillis = ERaMillis();
    if ((currentMillis - this->lastSelfInfo) < this->selfInfo) {
        return;
    }
    unsigned long skipTimes = ((currentMillis - this->lastSelfInfo) / this->selfInfo);
    // update time
    this->lastSelfInfo += (this->selfInfo * skipTimes);

    if (this->connected()) {
        this->sendSelfInfo();
    }
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::publishData(const char* prefixTopic, const char* payload,
                                                        bool retained, bool extended) {
    if (prefixTopic == nullptr) {
        return false;
    }
    if (payload == nullptr) {
        return false;
    }

    char topic[MAX_TOPIC_LENGTH] {0};
    FormatString(topic, this->ERA_TOPIC);
    FormatString(topic, prefixTopic);
    if (extended) {
        Handler::publishData(topic, payload);
    }
    return this->transp.publishData(topic, payload, retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::clearRetainedData(const char* prefixTopic) {
    return this->publishData(prefixTopic, "", true);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendListWiFi(const char* payload) {
    return this->publishData(ERA_PUB_PREFIX_LIST_WIFI_TOPIC,
                                            payload, false);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendChangeResultWiFi(const char* payload) {
    return this->publishData(ERA_PUB_PREFIX_CHANGE_RESULT_WIFI_TOPIC,
                                            payload, false);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendInfo() {
    bool status {false};
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    cJSON_AddStringToObject(root, INFO_ID, ERaMessageID::make());
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);

    Base::addInfo(root);

    cJSON_AddNumberToObject(root, INFO_PING, this->transp.getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason());
#endif

#if defined(ERA_ABBR)
    cJSON_AddBoolToObject(root, INFO_ABBR, true);
#else
    cJSON_AddBoolToObject(root, INFO_ABBR, false);
#endif

    if (this->heartbeat) {
        cJSON_AddNumberToObject(root, INFO_UPTIME, (ERaMillis() / 1000UL));
    }

    char* payload = cJSON_PrintUnformatted(root);

    status = this->publishData(ERA_PUB_PREFIX_INFO_TOPIC, payload,
                                        ERA_INFO_PUBLISH_RETAINED);

    cJSON_Delete(root);
    free(payload);
    root = nullptr;
    payload = nullptr;
    return status;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendSelfInfo() {
    bool status {false};
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

#if defined(ERA_MODBUS)
    cJSON_AddNumberToObject(root, SELF_MB_FAIL, Base::Modbus::getModbusFail());
    cJSON_AddNumberToObject(root, SELF_MB_TOTAL, Base::Modbus::getModbusTotal());
#else
    cJSON_AddNumberToObject(root, SELF_MB_FAIL, 0);
    cJSON_AddNumberToObject(root, SELF_MB_TOTAL, 0);
#endif
    cJSON_AddNumberToObject(root, SELF_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, SELF_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, SELF_TEMPERATURE, 0);

    Base::addSelfInfo(root);

    char* payload = cJSON_PrintUnformatted(root);

    status = this->publishData(ERA_PUB_PREFIX_SELF_SENSOR_TOPIC, payload,
                                            ERA_MQTT_PUBLISH_RETAINED);

    cJSON_Delete(root);
    free(payload);
    root = nullptr;
    payload = nullptr;
    return status;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendData(ERaRsp_t& rsp) {
    switch (rsp.type) {
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
        case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
        case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
        case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
        case ERaTypeWriteT::ERA_WRITE_PIN:
            return this->sendPinData(rsp);
        case ERaTypeWriteT::ERA_WRITE_CONFIG_ID:
        case ERaTypeWriteT::ERA_WRITE_CONFIG_ID_AND_TRIGGER:
            return this->sendConfigIdData(rsp);
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI:
        case ERaTypeWriteT::ERA_WRITE_CONFIG_ID_MULTI:
            return this->sendConfigIdMultiData(rsp);
        case ERaTypeWriteT::ERA_WRITE_NOTIFY:
            return this->sendNotifyData(rsp);
        case ERaTypeWriteT::ERA_WRITE_EMAIL:
            return this->sendEmailData(rsp);
        case ERaTypeWriteT::ERA_WRITE_WEBHOOK:
            return this->sendWebhookData(rsp);
        case ERaTypeWriteT::ERA_WRITE_SELF_DATA:
            return this->sendSelfData(rsp);
#if defined(ERA_MODBUS)
        case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA:
            return this->sendModbusData(rsp);
#endif
#if defined(ERA_ZIGBEE)
        case ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA:
            return this->sendZigbeeData(rsp);
#endif
#if defined(ERA_SPECIFIC)
        case ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA:
            return this->sendSpecificData(rsp);
#endif
        default:
            return false;
    }
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendPinData(ERaRsp_t& rsp) {
    ERaInt_t configId = rsp.id;
    int pMode = Base::getPinRp().findPinMode(rsp.id.getInt());
    switch (rsp.type) {
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN: {
                Base::getVirtualNextRetained(rsp.id.getInt(), rsp.retained);
                configId = Base::getPinRp().findVPinConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    break;
                }
                rsp.id = configId;
                this->sendConfigIdData(rsp);
                return true;
            }
            break;
        case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
            if ((pMode == OUTPUT) || (pMode == INPUT) ||
                (pMode == INPUT_PULLUP) || (pMode == INPUT_PULLDOWN)) {
                configId = Base::getPinRp().findConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    break;
                }
                rsp.id = configId;
                this->sendConfigIdData(rsp);
                return true;
            }
            break;
        case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
            if (pMode == ANALOG) {
                configId = Base::getPinRp().findConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    break;
                }
                rsp.id = configId;
                this->sendConfigIdData(rsp);
                return true;
            }
            break;
        case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
            if (pMode == PWM) {
                configId = Base::getPinRp().findConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    break;
                }
                rsp.id = configId;
                this->sendConfigIdData(rsp);
                return true;
            }
            break;
        case ERaTypeWriteT::ERA_WRITE_PIN:
            configId = Base::getPinRp().findConfigId(rsp.id.getInt(), rsp.param);
            if (configId < 0) {
                break;
            }
            rsp.id = configId;
            this->sendConfigIdData(rsp);
            return true;
        default:
            break;
    }

#if defined(ERA_PIN_DEBUG)
    char name[50] {0};
    bool status {false};
    char* payload = nullptr;
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    switch (rsp.type) {
    case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
        FormatString(name, "virtual_pin_" ERA_INTEGER_C_TYPE, rsp.id.getInt());
        break;
    case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
        FormatString(name, "digital_pin_" ERA_INTEGER_C_TYPE, rsp.id.getInt());
        break;
    case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
        FormatString(name, "analog_pin_" ERA_INTEGER_C_TYPE, rsp.id.getInt());
        break;
    case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
        FormatString(name, "pwm_pin_" ERA_INTEGER_C_TYPE, rsp.id.getInt());
        break;
    case ERaTypeWriteT::ERA_WRITE_PIN:
        FormatString(name, "pin_" ERA_INTEGER_C_TYPE, rsp.id.getInt());
        break;
    default:
        cJSON_Delete(root);
        root = nullptr;
        return false;
    }

    if (rsp.param.isNumber()) {
        cJSON_AddNumberWithDecimalToObject(root, name, rsp.param.getDouble(), ERA_DECIMAL_DOUBLE);
    }
    else if (rsp.param.isString()) {
        cJSON_AddStringToObject(root, name, rsp.param.getString());
    }
    else if (rsp.param.isObject() && (rsp.json || rsp.param.getObject()->isSendJSON())) {
        cJSON_AddItemToObject(root, name, rsp.param.getObject()->duplicateObject());
    }
    else if (rsp.param.isObject()) {
        cJSON_AddStringToObject(root, name, rsp.param.getObject()->getString());
    }
    payload = cJSON_PrintUnformatted(root);
    status = this->publishData(ERA_PUB_PREFIX_PIN_DATA_TOPIC, payload, rsp.retained);
    cJSON_Delete(root);
    free(payload);
    root = nullptr;
    payload = nullptr;
    return status;
#else
    return false;
#endif
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendPinMultiData(ERaRsp_t& rsp) {
    return this->publishData(ERA_PUB_PREFIX_PIN_DATA_TOPIC,
                                    rsp.param, rsp.retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendConfigIdData(ERaRsp_t& rsp) {
    bool status {false};
    char* payload = nullptr;
    char topicName[MAX_TOPIC_LENGTH] {0};
    FormatString(topicName, this->ERA_TOPIC);
    FormatString(topicName, ERA_PUB_PREFIX_CONFIG_DATA_TOPIC, rsp.id.getInt());

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }
    if (rsp.param.isNumber()) {
        cJSON_AddNumberWithDecimalToObject(root, "v", rsp.param.getDouble(), ERA_DECIMAL_DOUBLE);
    }
    else if (rsp.param.isString()) {
        cJSON_AddStringToObject(root, "v", rsp.param.getString());
    }
    else if (rsp.param.isObject() && (rsp.json || rsp.param.getObject()->isSendJSON())) {
        cJSON_AddItemToObject(root, "v", rsp.param.getObject()->duplicateObject());
    }
    else if (rsp.param.isObject()) {
        cJSON_AddStringToObject(root, "v", rsp.param.getObject()->getString());
    }
    payload = cJSON_PrintUnformatted(root);
    if (payload != nullptr) {
        status = this->transp.publishData(topicName, payload, rsp.retained);
        Handler::publishData(topicName, payload);
        this->sendConfigIdLogger(rsp, status, topicName, strlen(topicName));
    }
    cJSON_Delete(root);
    free(payload);
    root = nullptr;
    payload = nullptr;
    return status;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendConfigIdLogger(ERaRsp_t& rsp, bool status,
                                                 char* buf, size_t len) {
    if (this->pLogger == nullptr) {
        return false;
    }
    if (!rsp.param.isNumber()) {
        return false;
    }

    ERaDataBuff param(buf, 0, len);
    param.add(rsp.id.getInt());
    param.add(rsp.param.getDouble());
    this->pLogger->put("logger", param.at(0).getString(), param.at(1).getString(), status);
    return true;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendConfigIdMultiData(ERaRsp_t& rsp) {
    return this->publishData(ERA_PUB_PREFIX_MULTI_CONFIG_DATA_TOPIC,
                                            rsp.param, rsp.retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendNotifyData(ERaRsp_t& rsp) {
    char topic[MAX_TOPIC_LENGTH] {0};
    FormatString(topic, this->ERA_TOPIC);
    FormatString(topic, ERA_PUB_PREFIX_NOTIFY_DATA_TOPIC,
                 rsp.id.getInt(), rsp.param.getInt());
    return this->transp.publishData(topic, "{}", rsp.retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendEmailData(ERaRsp_t& rsp) {
    char topic[MAX_TOPIC_LENGTH] {0};
    FormatString(topic, this->ERA_TOPIC);
    FormatString(topic, ERA_PUB_PREFIX_EMAIL_DATA_TOPIC,
                 rsp.id.getInt(), rsp.param.getInt());
    return this->transp.publishData(topic, "{}", rsp.retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendWebhookData(ERaRsp_t& rsp) {
    char topic[MAX_TOPIC_LENGTH] {0};
    FormatString(topic, this->ERA_TOPIC);
    FormatString(topic, ERA_PUB_PREFIX_WEBHOOK_DATA_TOPIC,
                 rsp.id.getInt(), rsp.param.getInt());
    return this->transp.publishData(topic, "{}", rsp.retained);
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendSelfData(ERaRsp_t& rsp) {
    this->selfInfo = 0UL;
    return this->publishData(ERA_PUB_PREFIX_SELF_SENSOR_TOPIC,
                                    rsp.param, rsp.retained);
}

#if defined(ERA_AUTOMATION)
    template <class Transp, class Flash>
    bool ERaProto<Transp, Flash>::sendAutomation(ERaRsp_t& rsp) {
        if (!Base::hasAutomation()) {
            return false;
        }

        switch (rsp.type) {
            case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN: {
                ERaInt_t configId = Base::getPinRp().findVPinConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    return false;
                }
                rsp.id = configId;
                rsp.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
            }
                break;
            case ERaTypeWriteT::ERA_WRITE_PIN: {
                ERaInt_t configId = Base::getPinRp().findConfigId(rsp.id.getInt(), rsp.param);
                if (configId < 0) {
                    return false;
                }
                rsp.id = configId;
                rsp.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
            }
                break;
            case ERaTypeWriteT::ERA_WRITE_CONFIG_ID:
            case ERaTypeWriteT::ERA_WRITE_CONFIG_ID_AND_TRIGGER:
                break;
            default:
                return false;
        }

        Base::updateValueAutomation(rsp);
        return true;
    }
#endif

#if defined(ERA_MODBUS)
    template <class Transp, class Flash>
    bool ERaProto<Transp, Flash>::sendModbusData(ERaRsp_t& rsp) {
        bool status {false};
        char* payload = rsp.param;
        if (payload == nullptr) {
            return false;
        }

        status = this->publishData(ERA_PUB_PREFIX_MODBUS_DATA_TOPIC,
                                            payload, rsp.retained);
        if (this->pLogger != nullptr) {
            this->pLogger->put("modbus", "", payload, status);
        }
        payload = nullptr;
        return status;
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::clearRetainedModbusData() {
        this->clearRetainedData(ERA_PUB_PREFIX_MODBUS_DATA_TOPIC);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Transp, class Flash>
    bool ERaProto<Transp, Flash>::sendZigbeeData(ERaRsp_t& rsp) {
        bool status {false};
        char* id = rsp.id;
        char* payload = rsp.param;
        if (id == nullptr) {
            return false;
        }
        if (payload == nullptr) {
            id = nullptr;
            return false;
        }

        status = this->publishData(id, payload, rsp.retained, true);
        if (this->pLogger != nullptr) {
            this->pLogger->put("zigbee", id, payload, status, true);
        }
        id = nullptr;
        payload = nullptr;
        return status;
    }

    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::clearRetainedZigbeeData(const char* id) {
        this->clearRetainedData(id);
    }
#endif

#if defined(ERA_SPECIFIC)
    template <class Transp, class Flash>
    bool ERaProto<Transp, Flash>::sendSpecificData(ERaRsp_t& rsp) {
        bool status {false};
        char* id = rsp.id;
        char* payload = rsp.param;
        if (id == nullptr) {
            return false;
        }
        if (payload == nullptr) {
            id = nullptr;
            return false;
        }

        status = this->publishData(id, payload, rsp.retained, true);
        if (this->pLogger != nullptr) {
            this->pLogger->put("specific", id, payload, status, true);
        }
        id = nullptr;
        payload = nullptr;
        return status;
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommand(const char* auth, ERaRsp_t& rsp, ApiData_t data) {
    if (!this->connected()) {
        return;
    }
    if (auth == nullptr) {
        return;
    }

    char* payload = nullptr;
    char topicName[MAX_TOPIC_LENGTH] {0};
    FormatString(topicName, "%s/%s", BASE_TOPIC, auth);
    switch (rsp.type) {
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
            FormatString(topicName, ERA_PUB_PREFIX_VIRTUAL_DOWN_TOPIC, rsp.id.getInt());
            break;
        case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
        case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
        case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
        case ERaTypeWriteT::ERA_WRITE_PIN:
            FormatString(topicName, ERA_PUB_PREFIX_ARDUINO_DOWN_TOPIC, rsp.id.getInt());
            break;
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI: {
            ERaDataJson* vrData = (ERaDataJson*)data;
            this->sendCommandVirtualMulti(auth, rsp, vrData);
        }
            return;
#if defined(ERA_ZIGBEE)
        case ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA:
            this->sendCommandZigbee(auth, rsp);
            return;
#endif
        default:
            return;
    }

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    if (rsp.param.isNumber()) {
        cJSON_AddNumberWithDecimalToObject(root, "value", rsp.param.getDouble(), ERA_DECIMAL_DOUBLE);
    }
    else if (rsp.param.isString()) {
        cJSON_AddStringToObject(root, "value", rsp.param.getString());
    }
    else if (rsp.param.isObject() && (rsp.json || rsp.param.getObject()->isSendJSON())) {
        cJSON_AddItemToObject(root, "value", rsp.param.getObject()->duplicateObject());
    }
    else if (rsp.param.isObject()) {
        cJSON_AddStringToObject(root, "value", rsp.param.getObject()->getString());
    }
    payload = cJSON_PrintUnformatted(root);
    if (payload != nullptr) {
        this->transp.publishData(topicName, payload, rsp.retained);
    }
    cJSON_Delete(root);
    free(payload);
    root = nullptr;
    payload = nullptr;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommandVirtualMulti(const char* auth, ERaRsp_t& rsp, ERaDataJson* data) {
    if (data == nullptr) {
        return;
    }

    rsp.type = ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN;
    const ERaDataJson::iterator e = data->end();
    for (ERaDataJson::iterator it = data->begin(); it != e; ++it) {
        if (it.getName() == nullptr) {
            continue;
        }
        rsp.id = atoi(it.getName());
        if (it.isString()) {
            rsp.param = it.getString();
        }
        else if (it.isBool() || it.isNumber()) {
            rsp.param = it.getDouble();
        }
        else if (it.isObject() || it.isArray()) {
            rsp.param = it.getObject();
        }
        this->sendCommand(auth, rsp);
    }
}

#if defined(ERA_ZIGBEE)
    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::sendCommandZigbee(const char* auth, ERaRsp_t& rsp) {
        char* id = rsp.id;
        char* payload = rsp.param;
        if (auth == nullptr) {
            return;
        }
        if (id == nullptr) {
            return;
        }
        if (payload == nullptr) {
            id = nullptr;
            return;
        }

        char topicName[MAX_TOPIC_LENGTH] {0};
        FormatString(topicName, "%s/%s", BASE_TOPIC, auth);
        FormatString(topicName, ERA_PUB_PREFIX_ZIGBEE_DOWN_TOPIC, id);
        this->transp.publishData(topicName, payload, rsp.retained);
        id = nullptr;
        payload = nullptr;
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommand(ERaRsp_t& rsp, ApiData_t data) {
#if defined(ERA_AUTOMATION)
    this->sendAutomation(rsp);
#endif

    switch (rsp.type) {
        case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI: {
            ERaDataJson* vrData = (ERaDataJson*)data;
            this->sendCommandVirtual(rsp, vrData);
        }
            break;
#if defined(ERA_MODBUS)
        case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA: {
            ERaDataBuff* modbusData = (ERaDataBuff*)data;
            this->sendCommandModbus(rsp, modbusData);
        }
            break;
#endif
        default:
            break;
    }
    this->sendData(rsp);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommandVirtual(ERaRsp_t& rsp, ERaDataJson* data) {
    if (data == nullptr) {
        return;
    }

    ERaDataJson dataNoConfig;
    ERaDataJson::iterator current;
    const ERaDataJson::iterator e = data->end();
    for (ERaDataJson::iterator it = data->begin(); it != e;) {
        if (it.getName() == nullptr) {
            continue;
        }
        char name[2 + 8 * sizeof(ERaInt_t)] {0};
        ERaInt_t configId = Base::getPinRp().findVPinConfigId(atoi(it.getName()), it);
        if (configId < 0) {
            cJSON* item = data->detach(it);
            FormatString(name, "virtual_pin_%s", it.getName());
            dataNoConfig.add(name, item);
            if (!current) {
                it.shared(data->begin());
            }
            else {
                it.shared(current);
                ++it;
            }
            continue;
        }
        FormatString(name, ERA_INTEGER_C_TYPE, configId);
        it.rename(name);
        current.shared(it);
        ++it;
    }
    if (dataNoConfig.isValid()) {
        rsp.param = dataNoConfig.getObject();
        rsp.type = ERaTypeWriteT::ERA_WRITE_MAX;
        this->sendPinMultiData(rsp);
    }
    if (current) {
        rsp.param = data->getObject();
        rsp.type = ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI;
    }
}

#if defined(ERA_MODBUS)
    template <class Transp, class Flash>
    void ERaProto<Transp, Flash>::sendCommandModbus(ERaRsp_t& rsp, ERaDataBuff* data) {
        if (data == nullptr) {
            return;
        }
        if (data->isEmpty()) {
            return;
        }

        cJSON* root = cJSON_CreateObject();
        if (root == nullptr) {
            return;
        }
        int mbFailRead {0};
        int mbFailWrite {0};
        int mbTotalRead {0};
        int mbTotalWrite {0};
        size_t index {0};
        size_t dataLen = ERaMax(data->getLen(), data->getDataLen()) + 1;
        char* mbData = (char*)ERA_CALLOC(dataLen, sizeof(char));
        if (mbData == nullptr) {
            return;
        }
        char* mbAck = (char*)ERA_CALLOC(dataLen, sizeof(char));
        if (mbAck == nullptr) {
            return;
        }

        const char* mbScan = nullptr;
        const ERaDataBuff::iterator e = data->end(data->getDataLen());
        for (ERaDataBuff::iterator it = data->begin(data->getDataLen()); it < e; ++it) {
            if (it == MODBUS_STRING_FAIL_READ) {
                ++it;
                mbFailRead = it.getInt();
            }
            else if (it == MODBUS_STRING_FAIL_WRITE) {
                ++it;
                mbFailWrite = it.getInt();
            }
            else if (it == MODBUS_STRING_TOTAL_READ) {
                ++it;
                mbTotalRead = it.getInt();
            }
            else if (it == MODBUS_STRING_TOTAL_WRITE) {
                ++it;
                mbTotalWrite = it.getInt();
            }
            else if (it == MODBUS_STRING_SCAN) {
                ++it;
                mbScan = it.getString();
            }
            else if (IsHexString(it.getString())) {
                if (index++ % 2 == 0) {
                    FormatString(mbData, dataLen, it.getString());
                }
                else {
                    FormatString(mbAck, dataLen, it.getString());
                }
            }
        }
        cJSON_AddStringToObject(root, INFO_MB_DATA, mbData);
        cJSON_AddStringToObject(root, INFO_MB_ACK, mbAck);
        cJSON_AddNumberToObject(root, INFO_MB_FAIL_READ, mbFailRead);
        cJSON_AddNumberToObject(root, INFO_MB_FAIL_WRITE, mbFailWrite);
        cJSON_AddNumberToObject(root, INFO_MB_TOTAL_READ, mbTotalRead);
        cJSON_AddNumberToObject(root, INFO_MB_TOTAL_WRITE, mbTotalWrite);
        cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
        cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
        cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
        Base::addModbusInfo(root);
        if (mbScan != nullptr) {
            cJSON_AddStringToObject(root, INFO_MB_SCAN, mbScan);
        }
        free(mbData);
        free(mbAck);
        mbData = nullptr;
        mbAck = nullptr;
        if (index && ((index % 2) == 0)) {
            rsp.param = root;
        }
        cJSON_Delete(root);
        root = nullptr;
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::clearRetainedConfigIdData(ERaInt_t configId) {
    char topicName[MAX_TOPIC_LENGTH] {0};
    FormatString(topicName, this->ERA_TOPIC);
    FormatString(topicName, ERA_PUB_PREFIX_CONFIG_DATA_TOPIC, configId);
    this->transp.publishData(topicName, "", true);
}

template <class Transp, class Flash>
size_t ERaProto<Transp, Flash>::splitString(char* strInput, const char* delims) {
    if ((strInput == nullptr) ||
        (delims == nullptr)) {
        return 0;
    }
    if (!strlen(strInput)) {
        return 0;
    }

    size_t size {0};
    char* token = strtok(strInput, delims);
    while (token != nullptr) {
        ++size;
        token = strtok(nullptr, delims);
    }
    return size;
}

#endif /* INC_ERA_PROTOCOL_HPP_ */
