#ifndef INC_ERA_API_HPP_
#define INC_ERA_API_HPP_

#include <stdint.h>
#include <ERa/ERaPin.hpp>
#include <ERa/ERaPinDef.hpp>
#include <ERa/ERaData.hpp>
#include <ERa/ERaParam.hpp>
#include <ERa/ERaHooks.hpp>
#include <ERa/ERaHelpers.hpp>
#include <ERa/ERaPropertyDet.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaTimer.hpp>
#include <ERa/ERaApiHandler.hpp>
#include <Utility/ERaQueue.hpp>
#include <Modbus/ERaModbusSimple.hpp>
#include <Zigbee/ERaZigbeeSimple.hpp>
#include <Reason/ERaReason.hpp>
#include <ERa/ERaApiDef.hpp>

template <class Proto, class Flash>
class ERaApi
    : public ERaApiHandler
#if !defined(ERA_ABBR)
    , public ERaProperty< ERaApi<Proto, Flash> >
#endif
#if defined(ERA_MODBUS)
    , public ERaModbus< ERaApi<Proto, Flash> >
#endif
#if defined(ERA_ZIGBEE)
    , public ERaZigbee< ERaApi<Proto, Flash> >
#endif
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> FunctionCallback_t;
    typedef std::function<void(void*)> ReportPinCallback_t;
#else
    typedef void (*FunctionCallback_t)(void);
    typedef void (*ReportPinCallback_t)(void*);
#endif
    const char* TAG = "Api";
    const char* FILENAME_BT_CFG = FILENAME_BT_CONFIG;
    const char* FILENAME_PIN_CFG = FILENAME_PIN_CONFIG;

    typedef void* TaskHandle_t;
    typedef ERaApiHandler Handler;

protected:
#if !defined(ERA_ABBR)
    friend class ERaProperty< ERaApi<Proto, Flash> >;
    typedef ERaProperty< ERaApi<Proto, Flash> > Property;
#endif
#if defined(ERA_MODBUS)
    friend class ERaModbus< ERaApi<Proto, Flash> >;
    typedef ERaModbus< ERaApi<Proto, Flash> > Modbus;
#endif
#if defined(ERA_ZIGBEE)
    friend class ERaZigbee< ERaApi<Proto, Flash> >;
    typedef ERaZigbee< ERaApi<Proto, Flash> > Zigbee;
#endif

public:
    ERaApi(Flash& _flash)
        : flash(_flash)
        , ERaPinRp(ERaRp)
        , ledPin(-1)
        , invertLED(false)
        , skipPinWrite(false)
        , skipPinReport(false)
        , enableAppLoop(false)
        , taskSize(0UL)
        , autoSwitchTask(false)
#if !defined(ERA_NO_RTOS)
        , _apiTask(NULL)
#endif
    {}
    ~ERaApi()
    {}

    template <typename T>
    void virtualWrite(int pin, const T& value, bool send = false) {
#if defined(ERA_VIRTUAL_WRITE_LEGACY)
        this->virtualWriteSingle(pin, value, false);
        ERA_FORCE_UNUSED(send);
#else
        Property::virtualWriteProperty(pin, value, send);
#endif
    }

    template <typename T, typename... Args>
    void virtualWrite(int pin, const T& value, const Args&... tail) {
        this->virtualWriteMulti(pin, value, tail...);
    }

    template <typename... Args>
    void virtualObject(int pin, const Args&... tail) {
        ERaDataJson data;
        data.add_multi(tail...);
#if defined(ERA_VIRTUAL_WRITE_LEGACY)
        this->virtualWriteSingle(pin, data, ERA_API_JSON);
#else
        Property::virtualObjectProperty(pin, data, false);
#endif
    }

    template <typename... Args>
    void virtualArray(int pin, const Args&... tail) {
        ERaDataJson data;
        data.array_multi(tail...);
#if defined(ERA_VIRTUAL_WRITE_LEGACY)
        this->virtualWriteSingle(pin, data, ERA_API_JSON);
#else
        Property::virtualArrayProperty(pin, data, false);
#endif
    }

    void virtualObject(const char* value) {
        ERaDataJson data(value);
        this->virtualObject(data);
    }

    void virtualObject(cJSON* value) {
        ERaDataJson data(value);
        this->virtualObject(data);
    }

    void virtualObject(ERaDataJson& value) {
        this->virtualWriteMultiReal(value, false);
    }

    void digitalWrite(int pin, bool value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = pin;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    void analogWrite(int pin, int value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_ANALOG_PIN;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = pin;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    void pwmWrite(int pin, int value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_PWM_PIN;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = pin;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    void pinWrite(int pin, int value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_PIN;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = pin;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    template <typename T>
    void configIdWrite(ERaInt_t configId, const T& value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = configId;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    template <typename T, typename... Args>
    void configIdWrite(ERaInt_t configId, const T& value, const Args&... tail) {
        this->configIdMultiWrite(configId, value, tail...);
    }

    template <typename T>
    void configIdEvent(ERaInt_t configId, const T& value) {
#if ERA_MAX_EVENTS
        if (this->eventConfigIdAdd(configId, value)) {
            return;
        }
#endif
        this->configIdWrite(configId, value);
    }

#if defined(ERA_SPECIFIC)
    void specificWrite(const char* id, cJSON* value) override {
        return this->specificDataWrite(id, value, true, false);
    }

    void specificWrite(const char* id, const char* value) override {
        return this->specificDataWrite(id, value, true, false);
    }

    void specificDataWrite(const char* id, cJSON* value,
                           bool specific = false,
                           bool retained = ERA_MQTT_PUBLISH_RETAINED) override {
#if ERA_MAX_EVENTS
        if (this->eventSpecificAdd(id, value, specific, retained)) {
            return;
        }
#endif
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA;
        rsp.json = false;
        rsp.retained = retained;
        rsp.id = id;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
        ERA_FORCE_UNUSED(specific);
    }

    void specificDataWrite(const char* id, const char* value,
                           bool specific = false,
                           bool retained = ERA_MQTT_PUBLISH_RETAINED) override {
#if ERA_MAX_EVENTS
        if (this->eventSpecificAdd(id, value, specific, retained)) {
            return;
        }
#endif
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA;
        rsp.json = false;
        rsp.retained = retained;
        rsp.id = id;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
        ERA_FORCE_UNUSED(specific);
    }
#endif

    template <typename... Args>
    void selfWrite(const Args&... tail) {
        ERaDataJson data;
        data.add_multi(tail...);
        this->selfWriteReal(data);
    }

    void eraseAllConfigs() {
        this->beginFlash();
        this->removePinConfig();
#if defined(ERA_BT)
        this->removeBluetoothConfig();
#endif
#if defined(ERA_MODBUS)
        Modbus::removeConfigFromFlash();
#endif
    }

    void writeAllPin(ERaPin<ERaReport>::WritePinHandler_t writePin,
                    uint32_t value, uint8_t pMode, bool pwm = false) {
        this->ERaPinRp.writeAllPin(writePin, value, pMode, pwm);
    }

    /* Should be used very carefully */
    void delays(MillisTime_t ms) {
#if defined(ERA_NO_RTOS)
        if (!ms) {
            return;
        }
        MillisTime_t startMillis = ERaMillis();
        while (ERaRemainingTime(startMillis, ms)) {
            this->thisProto().run();
        }
#else
        ERaDelay(ms);
#endif
    }

    void osStarts() {
#if defined(ERA_HAS_RTOS) && !defined(ERA_NO_RTOS)
        ERaOs::osStartsScheduler();
#endif
    }

    void beginFlash() override {
        this->flash.begin();
    }

    char* readFromFlash(const char* filename, bool force = false) override {
        char* buf = nullptr;
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            buf = this->flash.readFlash(filename);
        }
        return buf;
    }

    void writeToFlash(const char* filename, const char* buf,
                                            bool force = false) override {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.writeFlash(filename, buf);
        }
    }

    size_t readBytesFromFlash(const char* key, void* buf, size_t maxLen,
                                            bool force = false) override {
        size_t size {0};
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            size = this->flash.readFlash(key, buf, maxLen);
        }
        return size;
    }

    void writeBytesToFlash(const char* key, const void* value, size_t len,
                                            bool force = false) override {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.writeFlash(key, value, len);
        }
    }

    Flash& getFlash() const {
        return this->flash;
    }

    void setSystemLED(int pin, bool invert = false);

    void setTaskSize(uint32_t size, bool switchTask = false) {
        if (!size) {
        }
        else if (size < ERA_API_TASK_SIZE) {
            size = ERA_API_TASK_SIZE;
        }
        this->taskSize = size;
        this->autoSwitchTask = switchTask;
    }

    void setSkipPinWrite(bool skip) {
        this->skipPinWrite = skip;
    }

    void setSkipPinReport(bool skip) {
        this->skipPinReport = skip;
    }

    void setAppLoop(bool enable) {
        this->enableAppLoop = enable;
    }

    void callERaProHandler(const char* deviceId, const cJSON* const root);

protected:
    void initApiTask();
    void initERaApiTask();
    void runERaApiTask();
    void addInfo(cJSON* root);
    void addSelfInfo(cJSON* root);
#if defined(ERA_MODBUS)
    void addModbusInfo(cJSON* root);
#endif
    void handleReadPin(cJSON* root);
    void handleWritePin(cJSON* root);
    void handleVirtualPin(cJSON* root);
    void handlePinRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processArduinoPinRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void processVirtualPinRequest(const ERaDataBuff& arrayTopic, const char* payload);
    void initPinConfig();
    void parsePinConfig(const char* str);
    void storePinConfig(const char* str);
    void removePinConfig();

#if defined(LINUX)
    static void* apiTask(void* args);
#else
    static void apiTask(void* args);
#endif

#if defined(ERA_BT)
    void initBluetoothConfig();
    void updateBluetoothConfig(char* buf);
    void parseBluetoothConfig(char* config, const char* hash, const char* buf);
    void storeBluetoothConfig(const char* str);
    void removeBluetoothConfig();
#endif

    void begin() {
        this->flash.begin();
        this->initPinConfig();
#if defined(ERA_BT)
        this->initBluetoothConfig();
#endif

        ERaWatchdogEnable(ERA_WATCHDOG_TIMEOUT);

        this->initERaApiTask();

        ERaWatchdogFeed();
    }

    bool run() {
        ERA_RUN_YIELD();

        if (!this->isTaskRunning()) {
            this->handlerAPI(true);
        }

        ERaWatchdogFeed();

        if (this->enableAppLoop) {
            ERaAppLoop();
        }
#if defined(ERA_NO_RTOS)
        else {
            ERaOnWaiting();
        }
#endif

        ERaWatchdogFeed();

#if ERA_MAX_EVENTS
        this->eventsWrite();
#endif
#if ERA_MAX_CLEARS
        this->clearsWrite();
#endif

        ERaWatchdogFeed();

        return this->thisProto().getTransp().run();
    }

    void handlerAPI(bool feed) {
#if !defined(ERA_ABBR)
        Property::run();
#endif
        this->ERaPinRp.run();

        if (feed) {
            ERaWatchdogFeed();
        }

        Handler::run();
    }

    void switchTask() {
        if (!this->autoSwitchTask) {
            return;
        }
        if (this->connected()) {
            return;
        }
        this->initApiTask();
    }

    void runAPI() {
        for (;;) {
            this->handlerAPI(false);
            ERA_API_YIELD();
            if (this->autoSwitchTask &&
                this->connected()) {
                break;
            }
        }
    }

    void runZigbee() {
#if defined(ERA_ZIGBEE)
        Zigbee::run();
#endif
    }

    void configIdClear(ERaInt_t configId) {
#if ERA_MAX_CLEARS
        if (this->clearConfigIdAdd(configId)) {
            return;
        }
#endif
        this->thisProto().clearRetainedConfigIdData(configId);
    }

#if defined(ERA_MODBUS)
    void modbusDataWrite(ERaDataBuff* value) {
#if ERA_MAX_EVENTS
        if (this->eventModbusAdd(value)) {
            return;
        }
#endif
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_MODBUS_DATA;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = 0;
        rsp.param = 0;
        this->thisProto().sendCommand(rsp, value);
    }

    void modbusDataClear() {
#if ERA_MAX_CLEARS
        if (this->clearModbusAdd()) {
            return;
        }
#endif
        this->thisProto().clearRetainedModbusData();
    }
#endif

#if defined(ERA_ZIGBEE)
    void zigbeeDataWrite(const char* id, cJSON* value,
                         bool specific = false,
                         bool retained = ERA_MQTT_PUBLISH_RETAINED) {
#if ERA_MAX_EVENTS
        if (this->eventZigbeeAdd(id, value, specific, retained)) {
            return;
        }
#endif
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA;
        rsp.json = false;
        rsp.retained = retained;
        rsp.id = id;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
        ERA_FORCE_UNUSED(specific);
    }

    void zigbeeDataClear(const char* id) {
#if ERA_MAX_CLEARS
        if (this->clearZigbeeAdd(id)) {
            return;
        }
#endif
        this->thisProto().clearRetainedZigbeeData(id);
    }
#endif

    void callERaWriteHandler(uint8_t pin, const ERaParam& param) override {
#if !defined(ERA_ABBR)
        Property::handler(pin, param);
#endif
        ERaWriteHandler_t handle = getERaWriteHandler(pin);
        if ((handle != nullptr) &&
            (handle != ERaWidgetWrite)) {
            handle(pin, param);
        }
        else {
            ERaWidgetWriteDefault(pin, param);
        }
    }

    void callERaPinReadHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) override {
        ERaPinReadHandler_t handle = getERaPinReadHandler(pin);
        if ((handle != nullptr) &&
            (handle != ERaWidgetPinRead)) {
            handle(pin, param, raw);
        }
        else {
            ERaWidgetPinReadDefault(pin, param, raw);
        }
    }

    bool callERaPinWriteHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) override {
        ERaPinWriteHandler_t handle = getERaPinWriteHandler(pin);
        if ((handle != nullptr) &&
            (handle != ERaWidgetPinWrite)) {
            return handle(pin, param, raw);
        }
        else {
            return ERaWidgetPinWriteDefault(pin, param, raw);
        }
    }

    void beginReadFromFlash(const char* filename, bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.beginRead(filename);
        }
    }

    char* readLineFromFlash(bool force = false) {
        char* buf = nullptr;
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            buf = this->flash.readLine();
        }
        return buf;
    }

    void endReadFromFlash(bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.endRead();
        }
    }

    void beginWriteToFlash(const char* filename, bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.beginWrite(filename);
        }
    }

    void writeLineToFlash(const char* buf, bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.writeLine(buf);
        }
    }

    void endWriteToFlash(bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.endWrite();
        }
    }

    void removeFlash(const char* filename, bool force = false) {
        if (!this->thisProto().getTransp().getAskConfig() || force) {
            this->flash.writeFlash(filename, "{}");
        }
    }

    ERaPin<ERaReport>& getPinRp() {
        return this->ERaPinRp;
    }

    void appLoop();
#if defined(ERA_HAS_FUNCTIONAL_H)
    FunctionCallback_t appCb = [&, this]() {
        this->appLoop();
    };
#else
    static void _appLoop();
    FunctionCallback_t appCb = []() {
        ERaApi::_appLoop();
    };
#endif

    bool isNeedSyncConfig() const {
        bool ret {false};
#if defined(ERA_ZIGBEE)
        ret |= Zigbee::isNeedFinalize();
#endif
        return ret;
    }

private:
    template <typename T>
    void virtualWriteSingle(int pin, const T& value, bool json = false,
                            bool retained = ERA_MQTT_PUBLISH_RETAINED) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN;
        rsp.json = json;
        rsp.retained = retained;
        rsp.id = pin;
        rsp.param = value;
        this->thisProto().sendCommand(rsp);
    }

    template <typename... Args>
    void virtualWriteMulti(const Args&... tail) {
        ERaDataJson data;
        data.add_multi(tail...);
        this->virtualWriteMultiReal(data, false);
    }

    void virtualWriteMultiReal(ERaDataJson& value, bool json = false) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI;
        rsp.json = json;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = 0;
        rsp.param = 0;
        this->thisProto().sendCommand(rsp, &value);
    }

    template <typename... Args>
    void configIdMultiWrite(const Args&... tail) {
        ERaRsp_t rsp;
        ERaDataJson data;
        data.add_multi(tail...);
        rsp.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID_MULTI;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = 0;
        rsp.param = data.getObject();
        this->thisProto().sendCommand(rsp);
    }

    void selfWriteReal(ERaDataJson& value) {
        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_SELF_DATA;
        rsp.json = true;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = 0;
        rsp.param = value.getObject();
        this->thisProto().sendCommand(rsp);
    }

    void onLED();
    void offLED();

    void getScaleConfig(const cJSON* const root, PinConfig_t& pin);
    void getReportConfig(const cJSON* const root, PinConfig_t& pin);
    void getPinConfig(const cJSON* const root, PinConfig_t& pin);
    uint8_t getPinMode(const cJSON* const root, const uint8_t defaultMode = VIRTUAL);
    bool isReadPinMode(uint8_t pMode);
    bool getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin);

    void sendPinEvent(void* args);
    void sendPinConfigEvent(void* args);
#if !defined(ERA_HAS_FUNCTIONAL_H)
    static void _sendPinEvent(void* args);
    static void _sendPinConfigEvent(void* args);
#endif

#if defined(ERA_NO_RTOS)
    bool isTaskRunning() const {
        return false;
    }
#else
    bool isTaskRunning() const {
        return ((this->taskSize > 0) && (this->_apiTask != NULL));
    }
#endif

    inline
    const Proto& thisProto() const {
        return static_cast<const Proto&>(*this);
    }

    inline
    Proto& thisProto() {
        return static_cast<Proto&>(*this);
    }

#if defined(ERA_HAS_FUNCTIONAL_H)
    ReportPinCallback_t reportPinCb = [&, this](void* args) {
        this->sendPinEvent(args);
    };

    ReportPinCallback_t reportPinConfigCb = [&, this](void* args) {
        this->sendPinConfigEvent(args);
    };
#else
    ReportPinCallback_t reportPinCb = [](void* args) {
        ERaApi::_sendPinEvent(args);
    };

    ReportPinCallback_t reportPinConfigCb = [](void* args) {
        ERaApi::_sendPinConfigEvent(args);
    };
#endif

#if ERA_MAX_EVENTS
    void eventsWrite();

    bool eventConfigIdAdd(ERaInt_t configId, ERaInt_t value,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    bool eventConfigIdAdd(ERaInt_t configId, double value,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    bool eventConfigIdAdd(ERaInt_t configId, const char* value,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    void eventConfigIdWrite(ERaEvent_t& event);

#if defined(ERA_MODBUS)
    bool eventModbusAdd(ERaDataBuff* value);
    void eventModbusWrite(ERaEvent_t& event);
#endif
#if defined(ERA_ZIGBEE)
    bool eventZigbeeAdd(const char* id, cJSON* value,
                        bool specific = false,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    void eventZigbeeWrite(ERaEvent_t& event);
#endif
#if defined(ERA_SPECIFIC)
    bool eventSpecificAdd(const char* id, cJSON* value,
                        bool specific = false,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    bool eventSpecificAdd(const char* id, const char* value,
                        bool specific = false,
                        bool retained = ERA_MQTT_PUBLISH_RETAINED);
    void eventSpecificWrite(ERaEvent_t& event);
#endif

    bool isEvent() {
        return this->queue.readable();
    }

    ERaEvent_t& getEvent() {
        return this->queue;
    }

    bool isEmptyEvent() {
        return this->queue.isEmpty();
    }

    ERaQueue<ERaEvent_t, ERA_MAX_EVENTS> queue;
#endif

#if ERA_MAX_CLEARS
    void clearsWrite();

    bool clearConfigIdAdd(ERaInt_t configId);
    void clearConfigIdWrite(ERaClear_t& clear);

#if defined(ERA_MODBUS)
    bool clearModbusAdd();
    void clearModbusWrite(ERaClear_t& clear);
#endif
#if defined(ERA_ZIGBEE)
    bool clearZigbeeAdd(const char* id);
    void clearZigbeeWrite(ERaClear_t& clear);
#endif

    bool isClear() {
        return this->queueClear.readable();
    }

    ERaClear_t& getClear() {
        return this->queueClear;
    }

    bool isEmptyClear() {
        return this->queueClear.isEmpty();
    }

    ERaQueue<ERaClear_t, ERA_MAX_CLEARS> queueClear;
#endif

    Flash& flash;
    ERaReport ERaRp;
    ERaPin<ERaReport> ERaPinRp;

    int ledPin;
    bool invertLED;
    bool skipPinWrite;
    bool skipPinReport;

    bool enableAppLoop;

    uint32_t taskSize;
    bool autoSwitchTask;

#if !defined(ERA_NO_RTOS)
    TaskHandle_t _apiTask;
#endif
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handleVirtualPin(cJSON* root) {
    if (!cJSON_IsArray(root)) {
        return;
    }

    PinConfig_t pin {};
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr; current = current->next) {
        if (!cJSON_IsObject(current)) {
            continue;
        }
        pin = PinConfig_t();
        cJSON* item = cJSON_GetObjectItem(current, "config_id");
        if (cJSON_IsNumber(item)) {
            pin.configId = item->valueint;
        }
        item = cJSON_GetObjectItem(current, "pin_number");
        if (cJSON_IsNumber(item)) {
            pin.pin = ERA_DECODE_PIN_NUMBER(item->valueint);
        }
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
        item = cJSON_GetObjectItem(current, "new_value_type");
        if (item == nullptr) {
            item = cJSON_GetObjectItem(current, "value_type");
        }
        if (cJSON_IsString(item)) {
            if (ERaStrCmp(item->valuestring, "number")) {
                this->ERaPinRp.setPinVirtual(pin.pin, pin.configId,
                                            ERaPin<ERaReport>::VIRTUAL_NUMBER);
            }
            else if (ERaStrCmp(item->valuestring, "string")) {
                this->ERaPinRp.setPinVirtual(pin.pin, pin.configId,
                                            ERaPin<ERaReport>::VIRTUAL_STRING);
            }
            else {
                this->ERaPinRp.setPinVirtual(pin.pin, pin.configId);
            }
        }
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::processVirtualPinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    if (arrayTopic.size() != 3) {
        return;
    }
    const char* str = arrayTopic.at(2).getString();
    if (str == nullptr) {
        return;
    }
    cJSON* root = cJSON_Parse(payload);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }
    ERaDataJson data(root);
    ERaParam param(data);
    uint8_t pin = ERA_DECODE_PIN_NAME(str);
    cJSON* item = cJSON_GetObjectItem(root, "value");
    if (cJSON_IsBool(item)) {
        param.add(item->valueint);
    }
    else if (cJSON_IsNumber(item)) {
        param.add(item->valuedouble);
    }
    else if (cJSON_IsString(item)) {
        param.add_static(item->valuestring);
    }
    else if (item != nullptr) {
        param.add(item);
    }

    if (cJSON_IsObject(item) ||
        cJSON_IsArray(item)) {
        data.detachObject();
        data.setObject(item, false);
    }

    this->callERaWriteHandler(pin, param);

    data.detachObject();
    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initPinConfig() {
    char* ptr = nullptr;
    ptr = this->readFromFlash(FILENAME_PIN_CFG);
    this->parsePinConfig(ptr);
    free(ptr);
    ptr = nullptr;
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::parsePinConfig(const char* str) {
    cJSON* root = cJSON_Parse(str);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "hash_id");
    if (cJSON_IsString(item)) {
        this->ERaPinRp.updateHashID(item->valuestring);
    }
    item = cJSON_GetObjectItem(root, "command");
    if (cJSON_IsString(item)) {
        this->thisProto().processDownCommand(str, root, item);
    }

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;

    ERA_LOG(TAG, ERA_PSTR("Pin configuration loaded from flash"));
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::storePinConfig(const char* str) {
    if (str == nullptr) {
        return;
    }

    this->writeToFlash(FILENAME_PIN_CFG, str);
    ERA_LOG(TAG, ERA_PSTR("Pin configuration stored to flash"));
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::removePinConfig() {
    this->ERaPinRp.deleteAll();
    this->removeFlash(FILENAME_PIN_CFG);
}

#if defined(ERA_BT)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::initBluetoothConfig() {
        char* ptr = nullptr;
        ptr = this->readFromFlash(FILENAME_BT_CFG);
        this->updateBluetoothConfig(ptr);
        free(ptr);
        ptr = nullptr;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::updateBluetoothConfig(char* buf) {
        cJSON* root = cJSON_Parse(buf);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }
        cJSON* data = cJSON_GetObjectItem(root, "data");
        cJSON* item = cJSON_GetObjectItem(data, "hash_id");
        if (cJSON_IsString(item)) {
            // Update later
        }
        item = cJSON_GetObjectItem(data, "bluetooth");
        if (cJSON_IsString(item)) {
            this->thisProto().Proto::Handler::begin(item->valuestring);
        }
        cJSON_Delete(root);
        root = nullptr;

        ERA_LOG(TAG, ERA_PSTR("Bluetooth configuration loaded from flash"));
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::parseBluetoothConfig(char* config, const char* hash, const char* buf) {
        this->thisProto().Proto::Handler::begin(config);
        // Hash ID update later
        this->storeBluetoothConfig(buf);
        ERA_FORCE_UNUSED(hash);
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::storeBluetoothConfig(const char* str) {
        if (str == nullptr) {
            return;
        }

        this->writeToFlash(FILENAME_BT_CFG, str);
        ERA_LOG(TAG, ERA_PSTR("Bluetooth configuration stored to flash"));
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::removeBluetoothConfig() {
        this->removeFlash(FILENAME_BT_CFG);
    }
#endif

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::getScaleConfig(const cJSON* const root, PinConfig_t& pin) {
    if (!cJSON_IsObject(root)) {
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "min");
    if (cJSON_IsNumber(item)) {
        pin.scale.min = item->valuedouble;
    }
    item = cJSON_GetObjectItem(root, "max");
    if (cJSON_IsNumber(item)) {
        pin.scale.max = item->valuedouble;
    }
    item = cJSON_GetObjectItem(root, "raw_min");
    if (cJSON_IsNumber(item)) {
        pin.scale.rawMin = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "raw_max");
    if (cJSON_IsNumber(item)) {
        pin.scale.rawMax = item->valueint;
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::getReportConfig(const cJSON* const root, PinConfig_t& pin) {
    if (!cJSON_IsObject(root)) {
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "interval");
    if (cJSON_IsNumber(item)) {
        pin.report.interval = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "min_interval");
    if (cJSON_IsNumber(item)) {
        pin.report.minInterval = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "max_interval");
    if (cJSON_IsNumber(item)) {
        pin.report.maxInterval = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "reportable_change");
    if (cJSON_IsNumber(item)) {
        pin.report.reportableChange = ((item->valuedouble == 0) ? 1.0f : item->valuedouble);
    }
    item = cJSON_GetObjectItem(root, "channel");
    if (cJSON_IsNumber(item)) {
        pin.pwm.channel = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "frequency");
    if (cJSON_IsNumber(item)) {
        pin.pwm.frequency = item->valueint;
    }
    item = cJSON_GetObjectItem(root, "resolution");
    if (cJSON_IsNumber(item)) {
        pin.pwm.resolution = item->valueint;
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::getPinConfig(const cJSON* const root, PinConfig_t& pin) {
    if (root == nullptr) {
        return;
    }
    cJSON* item = cJSON_GetObjectItem(root, "value_converter_configuration");
    if (!cJSON_IsObject(item)) {
        return;
    }

    this->getScaleConfig(item, pin);
    this->getReportConfig(item, pin);
}

template <class Proto, class Flash>
inline
uint8_t ERaApi<Proto, Flash>::getPinMode(const cJSON* const root, const uint8_t defaultMode) {
    uint8_t mode = defaultMode;
    if (root == nullptr) {
        return mode;
    }
    cJSON* item = cJSON_GetObjectItem(root, "pin_mode");
    if (item == nullptr) {
        item = cJSON_GetObjectItem(root, "value_converter");
    }
    if (!cJSON_IsString(item)) {
        return mode;
    }
    if (!strlen(item->valuestring)) {
        return mode;
    }

    if (ERaStrCmp(item->valuestring, "output")) {
        mode = OUTPUT;
    }
    else if (ERaStrCmp(item->valuestring, "open_drain")) {
        mode = OUTPUT_OPEN_DRAIN;
    }
    else if (ERaStrCmp(item->valuestring, "pwm")) {
        mode = PWM;
    }
    else if (ERaStrCmp(item->valuestring, "input")) {
        mode = INPUT;
    }
    else if (ERaStrCmp(item->valuestring, "pull_up")) {
        mode = INPUT_PULLUP;
    }
    else if (ERaStrCmp(item->valuestring, "pull_down")) {
        mode = INPUT_PULLDOWN;
    }
    else if (ERaStrCmp(item->valuestring, "analog")) {
        mode = ANALOG;
    }
    else if (ERaStrCmp(item->valuestring, "virtual")) {
        mode = VIRTUAL;
    }

    return mode;
}

template <class Proto, class Flash>
inline
bool ERaApi<Proto, Flash>::isReadPinMode(uint8_t pMode) {
    switch (pMode) {
        case INPUT:
        case INPUT_PULLUP:
#if !defined(ERA_IGNORE_INPUT_PULLDOWN)
        case INPUT_PULLDOWN:
#endif
        case ANALOG:
            return true;
        default:
            return false;
    }
}

template <class Proto, class Flash>
inline
bool ERaApi<Proto, Flash>::getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin) {
    if (root == nullptr || key == nullptr) {
        return false;
    }

    if (strncmp(root->string, key, strlen(key))) {
        return false;
    }

    char* ptr = strstr(root->string, key);

    if (strchr((ptr += strlen(key)), '_')) {
        const char* str = (strchr(ptr, '_') + 1);
        if (str != nullptr) {
            pin = ERA_DECODE_PIN_NAME(str);
        }
    }

    return true;
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::appLoop() {
    if (!this->enableAppLoop) {
        return;
    }
    ERaAppLoop();
    if (this->isTaskRunning()) {
        return;
    }
    this->handlerAPI(true);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::sendPinEvent(void* args) {
    ERaReport::ReportData_t* rp = (ERaReport::ReportData_t*)args;
    if (rp == nullptr) {
        return;
    }
    switch (rp->pinMode) {
        case ANALOG:
            this->analogWrite(rp->pin, rp->value);
            break;
        case INPUT:
        case INPUT_PULLUP:
#if !defined(ERA_IGNORE_INPUT_PULLDOWN)
        case INPUT_PULLDOWN:
#endif
        case OUTPUT:
            this->digitalWrite(rp->pin, rp->value);
            break;
        case PWM:
            this->pwmWrite(rp->pin, rp->value);
            break;
        default:
            break;
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::sendPinConfigEvent(void* args) {
    ERaReport::ReportData_t* rp = (ERaReport::ReportData_t*)args;
    if (rp == nullptr) {
        return;
    }
    ERaParam raw(rp->value);
    ERaParam param(rp->value);
    if (rp->scale.enable) {
        raw = ERaMapNumberRange(rp->value, (double)rp->scale.min, (double)rp->scale.max,
                                           (double)rp->scale.rawMin, (double)rp->scale.rawMax);
    }
    this->configIdWrite(rp->configId, rp->value);
    this->callERaPinReadHandler(rp->pin, param, raw);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::setSystemLED(int pin, bool invert) {
    this->ledPin = pin;
    this->invertLED = invert;

#if defined(ARDUINO) || \
    defined(WIRING_PI)
    if (this->ledPin >= 0) {
        pinMode(this->ledPin, OUTPUT);
        ::digitalWrite(this->ledPin, !this->invertLED ? LOW : HIGH);
    }
#endif

    this->onLED();
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::onLED() {
#if defined(ARDUINO) || \
    defined(WIRING_PI)
    if (this->ledPin >= 0) {
        ::digitalWrite(this->ledPin, !this->invertLED ? HIGH : LOW);
    }
#endif
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::offLED() {
#if defined(ARDUINO) || \
    defined(WIRING_PI)
    if (this->ledPin >= 0) {
        ::digitalWrite(this->ledPin, !this->invertLED ? LOW : HIGH);
    }
#endif
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::callERaProHandler(const char* deviceId, const cJSON* const root) {
    char id[65] {0};
    ERaDataJson data;
    cJSON* current = nullptr;
    for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
        ERaParam param;
        ClearArray(id);
        FormatString(id, "%s:%s", deviceId, current->string);
        if (cJSON_IsBool(current)) {
            param = current->valueint;
        }
        else if (cJSON_IsNumber(current)) {
            param = current->valuedouble;
        }
        else if (cJSON_IsString(current)) {
            param.add_static(current->valuestring);
        }
        else if (cJSON_IsObject(current) ||
                 cJSON_IsArray(current)) {
            data.setObject(current, false);
            param = current;
            param = data;
        }
        else {
            continue;
        }
#if !defined(ERA_ABBR)
        Property::handler(id, param);
#endif
        data.detachObject();
    }
}

#if ERA_MAX_EVENTS

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::eventsWrite() {
        if (!this->isEvent()) {
            return;
        }

        ERaEvent_t& event = this->getEvent();
        switch (event.type) {
            case ERaTypeWriteT::ERA_WRITE_CONFIG_ID:
                this->eventConfigIdWrite(event);
                break;
#if defined(ERA_MODBUS)
            case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA:
                this->eventModbusWrite(event);
                break;
#endif
#if defined(ERA_ZIGBEE)
            case ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA:
                this->eventZigbeeWrite(event);
                break;
#endif
#if defined(ERA_SPECIFIC)
            case ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA:
                this->eventSpecificWrite(event);
                break;
#endif
            default:
                break;
        }
    }

template <class Proto, class Flash>
inline
bool ERaApi<Proto, Flash>::eventConfigIdAdd(ERaInt_t configId, ERaInt_t value, bool retained) {
    return this->eventConfigIdAdd(configId, (double)value, retained);
}

template <class Proto, class Flash>
inline
bool ERaApi<Proto, Flash>::eventConfigIdAdd(ERaInt_t configId, double value, bool retained) {
    if (!this->queue.writeable()) {
        return false;
    }

    ERaInt_t* id = (ERaInt_t*)malloc(sizeof(ERaInt_t));
    if (id == nullptr) {
        return false;
    }
    (*id) = configId;

    double* data = (double*)malloc(sizeof(double));
    if (data == nullptr) {
        free(id);
        id = nullptr;
        return false;
    }
    (*data) = value;

    ERaEvent_t event;
    event.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
    event.specific = false;
    event.json = false;
    event.retained = retained;
    event.id = (void*)id;
    event.data = (void*)data;
    this->queue += event;
    return true;
}

template <class Proto, class Flash>
inline
bool ERaApi<Proto, Flash>::eventConfigIdAdd(ERaInt_t configId, const char* value, bool retained) {
    if (!this->queue.writeable()) {
        return false;
    }

    ERaInt_t* id = (ERaInt_t*)malloc(sizeof(ERaInt_t));
    if (id == nullptr) {
        return false;
    }
    (*id) = configId;

    char* data = ERaStrdup(value);
    if (data == nullptr) {
        free(id);
        id = nullptr;
        return false;
    }

    ERaEvent_t event;
    event.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
    event.specific = true;
    event.json = false;
    event.retained = retained;
    event.id = (void*)id;
    event.data = (void*)data;
    this->queue += event;
    return true;
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::eventConfigIdWrite(ERaEvent_t& event) {
    if ((event.id == nullptr) ||
        (event.data == nullptr)) {
        return;
    }

    ERaRsp_t rsp;
    rsp.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
    rsp.json = event.json;
    rsp.retained = event.retained;
    rsp.id = (*(static_cast<ERaInt_t*>(event.id)));
    if (!event.specific) {
        rsp.param = (*(static_cast<double*>(event.data)));
    }
    else {
        rsp.param.add_static((char*)event.data);
    }
    this->thisProto().sendCommand(rsp);
    free(event.id);
    free(event.data);
    event.id = nullptr;
    event.data = nullptr;
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::eventModbusAdd(ERaDataBuff* value) {
        if (!this->queue.writeable()) {
            return false;
        }

        ERaEvent_t event;
        event.type = ERaTypeWriteT::ERA_WRITE_MODBUS_DATA;
        event.specific = false;
        event.json = false;
        event.retained = ERA_MQTT_PUBLISH_RETAINED;
        event.id = nullptr;
        event.data = value;
        this->queue += event;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::eventModbusWrite(ERaEvent_t& event) {
        if (event.data == nullptr) {
            return;
        }

        ERaRsp_t rsp;
        ERaDataBuff* data = (ERaDataBuff*)event.data;
        rsp.type = ERaTypeWriteT::ERA_WRITE_MODBUS_DATA;
        rsp.json = false;
        rsp.retained = ERA_MQTT_PUBLISH_RETAINED;
        rsp.id = 0;
        rsp.param = 0;
        this->thisProto().sendCommand(rsp, data);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::eventZigbeeAdd(const char* id, cJSON* value,
                                            bool specific, bool retained) {
        if (!this->queue.writeable()) {
            return false;
        }

        ERaEvent_t event;
        event.type = ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA;
        event.specific = specific;
        event.json = false;
        event.retained = retained;
        if (!specific) {
            event.id = (void*)id;
            event.data = (void*)value;
            this->queue += event;
            return true;
        }

        event.id = ERaStrdup(id);
        if (event.id == nullptr) {
            return false;
        }
        event.data = cJSON_PrintUnformatted(value);
        if (event.data == nullptr) {
            free(event.id);
            event.id = nullptr;
            return false;
        }

        this->queue += event;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::eventZigbeeWrite(ERaEvent_t& event) {
        if ((event.id == nullptr) ||
            (event.data == nullptr)) {
            return;
        }

        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA;
        rsp.json = event.json;
        rsp.retained = event.retained;
        rsp.id.add_static((char*)event.id);
        if (!event.specific) {
            rsp.param = Zigbee::findDevicePayload((char*)event.id);
        }
        else {
            rsp.param.add_static((char*)event.data);
        }
        this->thisProto().sendCommand(rsp);
        if (event.specific) {
            free(event.id);
            free(event.data);
            event.id = nullptr;
            event.data = nullptr;
        }
    }
#endif

#if defined(ERA_SPECIFIC)
    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::eventSpecificAdd(const char* id, cJSON* value,
                                                bool specific, bool retained) {
        if (!this->queue.writeable()) {
            return false;
        }

        ERaEvent_t event;
        event.type = ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA;
        event.specific = specific;
        event.json = false;
        event.retained = retained;
        if (!specific) {
            event.id = (void*)id;
            event.data = (void*)value;
            this->queue += event;
            return true;
        }

        event.id = ERaStrdup(id);
        if (event.id == nullptr) {
            return false;
        }
        event.data = cJSON_PrintUnformatted(value);
        if (event.data == nullptr) {
            free(event.id);
            event.id = nullptr;
            return false;
        }

        this->queue += event;
        return true;
    }

    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::eventSpecificAdd(const char* id, const char* value,
                                                bool specific, bool retained) {
        if (!this->queue.writeable()) {
            return false;
        }

        ERaEvent_t event;
        event.type = ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA;
        event.specific = specific;
        event.json = false;
        event.retained = retained;
        if (!specific) {
            event.id = (void*)id;
            event.data = (void*)value;
            this->queue += event;
            return true;
        }

        event.id = ERaStrdup(id);
        if (event.id == nullptr) {
            return false;
        }
        event.data = ERaStrdup(value);
        if (event.data == nullptr) {
            free(event.id);
            event.id = nullptr;
            return false;
        }

        this->queue += event;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::eventSpecificWrite(ERaEvent_t& event) {
        if ((event.id == nullptr) ||
            (event.data == nullptr)) {
            return;
        }

        ERaRsp_t rsp;
        rsp.type = ERaTypeWriteT::ERA_WRITE_SPECIFIC_DATA;
        rsp.json = false;
        rsp.retained = event.retained;
        rsp.id.add_static((char*)event.id);
        if (!event.specific) {
            rsp.param = (cJSON*)event.data;
        }
        else {
            rsp.param.add_static((char*)event.data);
        }
        this->thisProto().sendCommand(rsp);
        if (event.specific) {
            free(event.id);
            free(event.data);
            event.id = nullptr;
            event.data = nullptr;
        }
    }
#endif

#endif

#if ERA_MAX_CLEARS

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::clearsWrite() {
        if (!this->connected()) {
            return;
        }
        if (!this->isClear()) {
            return;
        }

        ERaClear_t& clear = this->getClear();
        switch (clear.type) {
            case ERaTypeWriteT::ERA_WRITE_CONFIG_ID:
                this->clearConfigIdWrite(clear);
                break;
#if defined(ERA_MODBUS)
            case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA:
                this->clearModbusWrite(clear);
                break;
#endif
#if defined(ERA_ZIGBEE)
            case ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA:
                this->clearZigbeeWrite(clear);
                break;
#endif
            default:
                break;
        }
    }

    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::clearConfigIdAdd(ERaInt_t configId) {
        if (!this->queueClear.writeable()) {
            return false;
        }

        ERaClear_t clear;
        clear.type = ERaTypeWriteT::ERA_WRITE_CONFIG_ID;
        clear.configId = configId;
        this->queueClear += clear;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::clearConfigIdWrite(ERaClear_t& clear) {
        this->thisProto().clearRetainedConfigIdData(clear.configId);
    }

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::clearModbusAdd() {
        if (!this->queueClear.writeable()) {
            return false;
        }

        ERaClear_t clear;
        clear.type = ERaTypeWriteT::ERA_WRITE_MODBUS_DATA;
        clear.configId = 0L;
        this->queueClear += clear;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::clearModbusWrite(ERaClear_t& clear) {
        this->thisProto().clearRetainedModbusData();
        ERA_FORCE_UNUSED(clear);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Proto, class Flash>
    inline
    bool ERaApi<Proto, Flash>::clearZigbeeAdd(const char* id) {
        if (!this->queueClear.writeable()) {
            return false;
        }

        ERaClear_t clear;
        clear.type = ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA;
        clear.rawId = id;
        this->queueClear += clear;
        return true;
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::clearZigbeeWrite(ERaClear_t& clear) {
        this->thisProto().clearRetainedZigbeeData(clear.rawId);
    }
#endif

#endif

#if !defined(ERA_PIN_DEBUG)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::handlePinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
        ERA_FORCE_UNUSED(arrayTopic);
        ERA_FORCE_UNUSED(payload);
    }
#endif

template <typename T>
inline
void ERaAttachNoRTOSRun(T& last) {
    last.run();
}

template <typename T, typename... Args>
inline
void ERaAttachNoRTOSRun(T& head, Args&... tail) {
    ERaAttachNoRTOSRun(head);
    ERaAttachNoRTOSRun(tail...);
}

#if defined(ERA_NO_RTOS)
    #define ERA_ATTACH_RUN(...) ERA_WAITING() { ERaAttachNoRTOSRun(__VA_ARGS__); }
#else
    #define ERA_ATTACH_RUN(...)
#endif

#include <ERa/ERaPragma.hpp>
#include <ERa/ERaWdt.hpp>

#endif /* INC_ERA_API_HPP_ */
