#ifndef INC_ERA_MODBUS_HPP_
#define INC_ERA_MODBUS_HPP_

#include <math.h>
#include <ERa/ERaParam.hpp>
#include <Utility/ERaUtility.hpp>
#include <Modbus/ERaModbusState.hpp>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaModbusConfig.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusStream.hpp>
#include <Modbus/ERaModbusTransp.hpp>
#include <Modbus/ERaModbusAction.hpp>
#include <Modbus/ERaModbusCallbacks.hpp>

using namespace std;

template <class Api>
class ERaModbus
    : public ERaModbusStream
    , public ERaModbusAction
    , public ERaModbusTransp < ERaModbus<Api> >
    , public ERaModbusSlave < ERaModbus<Api> >
{
    typedef struct __TCPIp_t {
        IPAddress ip;
        uint16_t port;
    } TCPIp_t;
    typedef void* TaskHandle_t;
    typedef void* QueueMessage_t;
    const char* TAG = "Modbus";
    const char* FILENAME_CONFIG = FILENAME_MODBUS_CONFIG;
    const char* FILENAME_CONTROL = FILENAME_MODBUS_CONTROL;
    const IPAddress ipNone{0, 0, 0, 0};

    typedef ERaModbusStream ModbusStream;
    typedef ERaModbusAction ModbusAction;

    friend class ERaModbusTransp < ERaModbus<Api> >;
    typedef ERaModbusTransp < ERaModbus<Api> > ModbusTransp;

    friend class ERaModbusSlave < ERaModbus<Api> >;
    typedef ERaModbusSlave < ERaModbus<Api> > ModbusSlave;

public:
    ERaModbus()
        : initialized(false)
#if !defined(ERA_NO_RTOS)
        , timerFatal()
#endif
        , modbusConfig(ERaModbusEntry::config())
        , modbusControl(ERaModbusEntry::control())
        , modbusScan(ERaScanEntry::instance())
        , pModbusCallbacks(NULL)
        , timeout(DEFAULT_TIMEOUT_MODBUS)
        , prevMillis(0)
        , predelay(0)
        , postdelay(0)
        , failRead(0)
        , failWrite(0)
        , totalRead(0)
        , totalWrite(0)
        , dePin(-1)
        , transp(0)
        , streamRTU(NULL)
        , clientTCP(NULL)
        , ip {
            .ip = {},
            .port = 502
        }
        , failCounter {
            .min = 0,
            .max = 255,
            .start = 0
        }
#if !defined(ERA_NO_RTOS)
        , _modbusTask(NULL)
        , _writeModbusTask(NULL)
#endif
        , messageHandle(NULL)
        , mutex(NULL)
        , skipModbus(false)
        , skipPubModbus(false)
        , wifiConfig(false)
        , actionWriteTask(true)
        , runApiResponse(true)
    {}
    ~ERaModbus()
    {
        this->switchToModbusRTU();
    }

    void setModbusClient(Client& _client, IPAddress _ip = IPAddress(0, 0, 0, 0), uint16_t _port = 502) {
        this->clientTCP = &_client;
        this->ip.ip = _ip;
        this->ip.port = _port;
    }

    void setModbusStream(Stream& _stream) {
        this->streamRTU = &_stream;
        ModbusStream::stream = &_stream;
    }

    void setModbusTimeout(uint32_t _timeout) {
        this->timeout = _timeout;
    }

    void setModbusInterval(uint32_t _interval) {
        if (!_interval) {
            return;
        }
        ERaModbusEntry::getConfig()->modbusInterval.delay = _interval;
    }

    void setPubModbusInterval(uint32_t _interval) {
        if (!_interval) {
            return;
        }
        ERaModbusEntry::getConfig()->pubInterval.delay = _interval;
    }

    void setModbusCallbacks(ERaModbusCallbacks& callbacks) {
        this->pModbusCallbacks = &callbacks;
    }

    void setModbusCallbacks(ERaModbusCallbacks* pCallbacks) {
        this->pModbusCallbacks = pCallbacks;
    }

    void writeAllModbus(uint8_t len1, uint8_t len2, const uint8_t* pData = NULL,
                        size_t pDataLen = 0, bool force = false, bool execute = false);
    void setModbusDEPin(int _pin);

    void setModbusDelays(unsigned long preUs, unsigned long postUs) {
        this->predelay = preUs;
        this->postdelay = postUs;
    }

    void setModbusFailCounter(uint8_t min, uint8_t max = 255, uint8_t start = 0) {
        this->failCounter.min = min;
        this->failCounter.max = max;
        this->failCounter.start = start;
    }

    void setSkipModbus(bool skip) {
        this->skipModbus = skip;
    }

    void setSkipPublishModbus(bool skip) {
        this->skipPubModbus = skip;
    }

    void setConnectWiFiModbus(bool enable) {
        this->wifiConfig = enable;
    }

    void setActionOnReadTaskModbus(bool enable) {
        this->actionWriteTask = !enable;
    }

protected:
    void begin() {
        ERaModbusEntry::getConfig();
        ERaModbusEntry::getControl();
        this->dataBuff.allocate(MODBUS_DATA_BUFFER_SIZE);
        this->initModbusConfig();
    }

    void run() {
        if (!this->initialized) {
            return;
        }
        if (this->skipModbus) {
            return;
        }
        if ((this->modbusConfig == nullptr) ||
            (this->modbusControl == nullptr)) {
            return;
        }

        this->runRead(false);
        this->runWrite(false);
        ERA_MODBUS_YIELD();
    }

    void runRead(bool forever = true) {
        this->waitTCPIpReady(!forever);
        for (;;) {
            this->processModbusScan();
            this->resizeConfig();
            if (!ERaRemainingTime(this->modbusConfig->modbusInterval.prevMillis, this->modbusConfig->modbusInterval.delay)) {
                this->modbusConfig->modbusInterval.prevMillis = ERaMillis();
                this->readModbusConfig();
                this->getModbusAction(false);
                this->getModbusActionRaw(false);
                ModbusStream::streamStart();
            }
            else {
                this->waitRequest();
            }
#if !defined(ERA_NO_RTOS)
            this->timer.run();
#endif
            if (!forever) {
                break;
            }
            ERA_MODBUS_YIELD();
        }
    }

    void runWrite(bool forever = true) {
        this->waitTCPIpReady(!forever);
        for (;;) {
            this->getModbusAction(true);
            this->getModbusActionRaw(true);
#if defined(ERA_PNP_MODBUS)
            this->processModbusControl();
#endif
            this->writeAllModbusWithOption();
            if (!forever) {
                break;
            }
            ERA_MODBUS_YIELD();
        }
    }

    void parseModbusConfig(const void* config, const char* hash, const char* buf,
                                    bool isControl = false, bool json = false) {
        ModbusState::set(ModbusStateT::STATE_MB_PARSE);
        if (isControl) {
            ERaGuardLock(this->mutex);
            this->modbusControl->parseConfig(config, json);
            if (this->modbusControl->updateHashID(hash)) {
                this->thisApi().writeToFlash(FILENAME_CONTROL, buf);
                ERaWriteConfig(ERaConfigTypeT::ERA_MODBUS_CONTROL);
#if !defined(ERA_PNP_MODBUS)
                this->modbusControl->updated();
#endif
            }
            ModbusState::set(ModbusStateT::STATE_MB_PARSE);
            ERaGuardUnlock(this->mutex);
            this->initModbus();
        }
        else {
            ERaGuardLock(this->mutex);
            this->modbusConfig->parseConfig(config, json);
            ModbusTransp::parseConfig(config, json);
            if (this->modbusConfig->updateHashID(hash)) {
                this->clearDataBuff();
                this->thisApi().writeToFlash(FILENAME_CONFIG, buf);
                if (this->wifiConfig && this->modbusConfig->isWiFi) {
                    this->thisApi().connectNewWiFi(this->modbusConfig->ssid,
                                                   this->modbusConfig->pass);
                }
                ERaWriteConfig(ERaConfigTypeT::ERA_MODBUS_CONFIG);
                this->modbusConfig->updated();
            }
            ModbusState::set(ModbusStateT::STATE_MB_PARSE);
            ERaGuardUnlock(this->mutex);
            this->initModbus();
            if (this->initialized) {
                ModbusStream::setBaudRate(this->modbusConfig->baudSpeed);
            }
            this->executeNow();
        }
    }

    void parseModbusScan(const char* config) {
        this->modbusScan->getInstance();
        this->modbusScan->parseConfig(config);
        ModbusState::set(ModbusStateT::STATE_MB_SCAN);
    }

    void removeConfigFromFlash() {
        ModbusState::set(ModbusStateT::STATE_MB_PARSE);
        ERaGuardLock(this->mutex);
        if (this->modbusConfig != nullptr) {
            this->modbusConfig->deleteAll();
        }
        if (this->modbusControl != nullptr) {
            this->modbusControl->deleteAll();
        }
        ModbusState::set(ModbusStateT::STATE_MB_PARSE);
        ERaGuardUnlock(this->mutex);
        this->thisApi().removeFlash(FILENAME_CONFIG);
        this->thisApi().removeFlash(FILENAME_CONTROL);
    }

    void configIdModbusWrite(ERaInt_t configId, float value) override {
        this->thisApi().configIdWrite(configId, value);
    }

    void configIdModbusRemove(ERaInt_t configId) override {
        this->thisApi().configIdRemove(configId);
    }

    void clearDataBuff() {
        this->dataBuff.clearBuffer();
    }

    int getModbusFail() const {
        return (this->failRead + this->failWrite);
    }

    int getModbusTotal() const {
        return (this->totalRead + this->totalWrite);
    }

    void initModbusTask();

#if defined(LINUX)
    static void* modbusTask(void* args);
    static void* writeModbusTask(void* args);
#else
    static void modbusTask(void* args);
    static void writeModbusTask(void* args);
#endif

private:
    void initModbus(bool skip = false) {
        if (this->initialized) {
            if (this->isEmptyConfig()) {
#if !defined(ERA_NO_RTOS)
                if (this->timerFatal == false) {
                    this->timerFatal = this->timer.setTimeout(ERA_FATALITY_TIMEOUT, [](void) {
                        ERaFatality();
                    });
                }
#endif
                ModbusStream::end();
                this->initialized = false;
            }
            return;
        }
        else if (this->isEmptyConfig()) {
            return;
        }

        if (skip) {
            return;
        }

#if !defined(ERA_NO_RTOS)
        if (this->timerFatal == true) {
            this->timerFatal.deleteTimer();
        }
#endif

        this->switchToModbusRTU();
        ModbusStream::begin();
#if !defined(ERA_NO_RTOS)
        if (!this->skipModbus &&
            !this->isTaskRunning()) {
            this->initModbusTask();
        }
#else
        if (!this->skipModbus) {
            this->initModbusTask();
        }
#endif
        this->initialized = true;
    }

    void initModbusConfig() {
        char* ptr = nullptr;
        ptr = this->thisApi().readFromFlash(FILENAME_CONFIG);
        this->updateConfig(this->modbusConfig, ptr, "configuration", "modbus_configuration", false);
        if (this->initialized) {
            ModbusStream::setBaudRate(this->modbusConfig->baudSpeed);
        }
        free(ptr);
        ptr = this->thisApi().readFromFlash(FILENAME_CONTROL);
        this->updateConfig(this->modbusControl, ptr, "control", "modbus_control", true);
        free(ptr);
        ptr = nullptr;
    }

    void updateConfig(ERaModbusEntry* config, const char* buf, const char* name,
                                    const char* jsName, bool isControl = false) {
        cJSON* root = cJSON_Parse(buf);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }

        cJSON* data = cJSON_GetObjectItem(root, "data");
        cJSON* item = cJSON_GetObjectItem(data, name);
        if (item == nullptr) {
            item = cJSON_GetObjectItem(data, jsName);
        }
        if (cJSON_IsString(item)) {
            config->parseConfig(item->valuestring, false);
        }
        else if (cJSON_IsObject(item)) {
            config->parseConfig(item, true);
            if (!isControl) {
                ModbusTransp::parseConfig(item, true);
            }
        }

        item = cJSON_GetObjectItem(data, "hash_id");
        if (cJSON_IsString(item)) {
            config->updateHashID(item->valuestring, true);
        }

        cJSON_Delete(root);
        root = nullptr;
        data = nullptr;
        item = nullptr;

        this->initModbus();
    }

    void resizeConfig() {
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            this->modbusConfig->resize();
            this->modbusControl->resize();
            this->initModbus(true);
            ERaDelay(500);
            if (ModbusTransp::isNewReport()) {
                this->thisApi().modbusDataRemove();
            }
            else {
                ModbusTransp::removeConfigId();
            }
        }
        ModbusState::set(ModbusStateT::STATE_MB_RUNNING);
    }

    void nextTransport(const ModbusConfig_t& param) {
        this->switchToModbusRTU();
        if (this->clientTCP == nullptr) {
            return;
        }
        if (!param.ipSlave.ip.dword) {
            return;
        }
        if (!param.ipSlave.port) {
            return;
        }
        if (!this->thisApi().afterNetwork()) {
            return;
        }
        this->switchToModbusTCP();
        if (this->ip.ip != IPAddress(param.ipSlave.ip.dword)) {
        }
        else if (this->clientTCP->connected()) {
            return;
        }
        this->ip.ip = param.ipSlave.ip.dword;
        this->ip.port = param.ipSlave.port;
        // TODO
        this->connectTcpIp(2);
    }

    void connectTcpIp(size_t retry) {
        if (this->clientTCP->connected()) {
            this->clientTCP->stop();
        }

        do {
            this->clientTCP->connect(this->ip.ip, this->ip.port);
            if (this->clientTCP->connected()) {
                break;
            }
            ERA_LOG(TAG, "Connect to %d.%d.%d.%d:%d failed", this->ip.ip[0], this->ip.ip[1],
                                                             this->ip.ip[2], this->ip.ip[3],
                                                             this->ip.port);
            ERaDelay(1000);
        } while (--retry);
    }

    void switchToModbusRTU() {
        ModbusStream::stream = this->streamRTU;
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_RTU;
    }

    void switchToModbusTCP() {
        ModbusStream::stream = this->clientTCP;
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_TCP;
    }

    void getModbusAction(bool writeTask) {
        if (this->actionWriteTask != writeTask) {
            return;
        }
        if (!ModbusAction::isRequest()) {
            return;
        }
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            return;
        }
        ModbusAction_t& req = ModbusAction::getRequest();
        if (req.key == nullptr) {
            return;
        }
        this->actionModbus(req);
        if (ModbusAction::isEmptyRequest()) {
            this->executeNow();
            if (!ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                ModbusState::set(ModbusStateT::STATE_MB_CONTROLLED);
            }
        }
        free(req.key);
        req.key = nullptr;
    }

    void getModbusActionRaw(bool writeTask) {
        if (this->actionWriteTask != writeTask) {
            return;
        }
        if (!ModbusAction::isRawRequest()) {
            return;
        }
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            return;
        }
        ModbusActionRaw_t& req = ModbusAction::getRawRequest();
        this->actionModbusRaw(req);
        if (ModbusAction::isEmptyRawRequest()) {
            this->executeNow();
            if (!ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                ModbusState::set(ModbusStateT::STATE_MB_CONTROLLED);
            }
        }
        if (req.pExtra != nullptr) {
            free(req.pExtra);
            req.pExtra = nullptr;
        }
    }

    void readModbusConfig();
    bool checkPubDataInterval();
    void delayModbus(ERaUInt_t address, bool unlock = false, bool skip = false);
    void delays(MillisTime_t ms);
    void delaysMutex(MillisTime_t ms);
    ModbusConfigAlias_t* getModbusAlias(const char* key);
    ModbusConfig_t* getModbusConfig(ERaUInt_t id);
    ModbusConfig_t* getModbusConfigWithAddress(uint8_t addr);
    void addScanData();
    void processModbusScan();
#if defined(ERA_PNP_MODBUS)
    void processModbusControl();
#endif
    void writeAllModbusWithOption(bool execute = false);
    bool actionModbus(ModbusAction_t& request);
    bool eachActionModbus(ModbusAction_t& request, Action_t& action, ModbusConfig_t*& config);
    bool actionModbusRaw(ModbusActionRaw_t& request);
    void sendModbusRead(ModbusConfig_t& param);
    bool handlerModbusRead(ModbusConfig_t* param);
    bool sendModbusWrite(ModbusConfig_t& param);
    bool handlerModbusWrite(ModbusConfig_t* param);
    void onData(ERaModbusRequest* request, ERaModbusResponse* response, bool skip = false);
    void onError(ERaModbusRequest* request, bool skip = false);
    void processStateBegin(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data);
    void processStateAddr(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data);
    void processStateFunc(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data);
    void processStateData(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data);
    void processInput(ERaModbusRequest* request, ERaModbusResponse* response);
    bool waitResponse(ERaModbusRequest* request, ERaModbusResponse* response);
    void waitRequest();
    void sendRequest(const uint8_t* data, size_t size);
    void sendCommand(const uint8_t* data, size_t size);
    void switchToTransmit();
    void switchToReceive();

    bool isRTU() {
        return (this->transp == ModbusTransportT::MODBUS_TRANSPORT_RTU);
    }

    bool isEmptyConfig() {
        return (this->modbusConfig->modbusConfigParam.isEmpty() &&
                this->modbusControl->modbusConfigParam.isEmpty());
    }

#if !defined(ERA_NO_RTOS)
    bool isTaskRunning() {
        return ((this->_modbusTask != NULL) &&
                (this->_writeModbusTask != NULL));
    }
#endif

    void updateTotalRead() {
        if (this->totalRead++ > 99) {
            this->totalRead = 1;
            this->failRead = 0;
        }
    }

    void updateTotalWrite() {
        if (this->totalWrite++ > 99) {
            this->totalWrite = 1;
            this->failWrite = 0;
        }
    }

    void waitTCPIpReady(bool skip) {
        if (skip) {
            return;
        }
        if (this->clientTCP == nullptr) {
            return;
        }
        if (!this->modbusConfig->hasTcpIp &&
            !this->modbusControl->hasTcpIp) {
            return;
        }
        MillisTime_t startMillis = ERaMillis();
        do {
            ERaDelay(1000);
            if (this->thisApi().afterNetwork()) {
                break;
            }
        } while (ERaRemainingTime(startMillis, ERA_MODBUS_WAIT_TCP_MS));
    }

    bool getBit(uint8_t byte, uint8_t pos) {
        return ((byte >> pos) & 0x01);
    }

    void executeNow() {
        this->modbusConfig->modbusInterval.prevMillis = (ERaMillis() - this->modbusConfig->modbusInterval.delay + ERA_MODBUS_EXECUTE_MS);
    }

    inline
    const Api& thisApi() const {
        return static_cast<const Api&>(*this);
    }

    inline
    Api& thisApi() {
        return static_cast<Api&>(*this);
    }

    bool initialized;

#if !defined(ERA_NO_RTOS)
    ERaTimer timer;
    ERaTimer::iterator timerFatal;
#endif

    ERaDataBuffDynamic dataBuff;
    ERaModbusEntry*& modbusConfig;
    ERaModbusEntry*& modbusControl;
    ERaScanEntry*& modbusScan;
    ERaModbusCallbacks* pModbusCallbacks;
    uint32_t timeout;
    unsigned long prevMillis;
    unsigned long predelay;
    unsigned long postdelay;
    int failRead;
    int failWrite;
    int totalRead;
    int totalWrite;
    int dePin;
    uint8_t transp;
    Stream* streamRTU;
    Client* clientTCP;
    TCPIp_t ip;

    struct {
        uint8_t min;
        uint8_t max;
        uint8_t start;
    } failCounter;

#if !defined(ERA_NO_RTOS)
    TaskHandle_t _modbusTask;
    TaskHandle_t _writeModbusTask;
#endif

    QueueMessage_t messageHandle;
    ERaMutex_t mutex;
    bool skipModbus;
    bool skipPubModbus;
    bool wifiConfig;
    bool actionWriteTask;
    volatile bool runApiResponse;
};

template <class Api>
void ERaModbus<Api>::readModbusConfig() {
    if (this->modbusConfig->modbusConfigParam.isEmpty()) {
        return;
    }
    this->dataBuff.clear();
    const ERaList<ModbusConfig_t*>::iterator* e = this->modbusConfig->modbusConfigParam.end();
    for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusConfig->modbusConfigParam.begin(); it != e; it = it->getNext()) {
        ModbusConfig_t* param = it->get();
        if (!this->handlerModbusRead(param)) {
            return;
        }
    }
    if (this->dataBuff.isEmpty()) {
        return;
    }
#if !defined(ERA_MODBUS_DATA_LEGACY)
    if (this->dataBuff.isChange()) {
        this->prevMillis = ERaMillis();
    }
    else if (!this->checkPubDataInterval()) {
        return;
    }
#endif
    this->dataBuff.add_multi(ERA_F(MODBUS_STRING_FAIL_READ), this->failRead,
                            ERA_F(MODBUS_STRING_FAIL_WRITE), this->failWrite,
                            ERA_F(MODBUS_STRING_TOTAL_READ), this->totalRead,
                            ERA_F(MODBUS_STRING_TOTAL_WRITE), this->totalWrite);
    this->addScanData();
    this->dataBuff.done();

    if (this->skipPubModbus || ModbusTransp::isNewReport()) {
        return;
    }
    this->thisApi().modbusDataWrite(&this->dataBuff);
}

template <class Api>
bool ERaModbus<Api>::checkPubDataInterval() {
    unsigned long currentMillis = ERaMillis();
    if ((currentMillis - this->prevMillis) < this->modbusConfig->pubInterval.delay) {
        return false;
    }
    unsigned long skipTimes = ((currentMillis - this->prevMillis) / this->modbusConfig->pubInterval.delay);
    this->prevMillis += (this->modbusConfig->pubInterval.delay * skipTimes);
    return true;
}

template <class Api>
void ERaModbus<Api>::delayModbus(ERaUInt_t address, bool unlock, bool skip) {
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        if (unlock) {
            ERaGuardUnlock(this->mutex);
        }
        return;
    }

    MillisTime_t delayMs {ERA_MODBUS_DELAYS_MS};
    const ERaList<SensorDelay_t*>::iterator* e = this->modbusConfig->sensorDelay.end();
    for (ERaList<SensorDelay_t*>::iterator* it = this->modbusConfig->sensorDelay.begin(); it != e; it = it->getNext()) {
        SensorDelay_t* param = it->get();
        if (param == nullptr) {
            continue;
        }
        if (param->address == address) {
            delayMs = param->delay;
            break;
        }
    }

    if (!unlock) {
        this->delays(delayMs);
        return;
    }

    if (delayMs <= ERA_MODBUS_MUTEX_MS) {
        this->delays(delayMs);
        ERaGuardUnlock(this->mutex);
        if (!skip) {
            ERA_MODBUS_YIELD();
        }
        return;
    }

    delayMs -= ERA_MODBUS_MUTEX_MS;
    this->delays(ERA_MODBUS_MUTEX_MS);
    ERaGuardUnlock(this->mutex);
    if (!skip) {
        this->delays(delayMs);
    }
}

template <class Api>
void ERaModbus<Api>::delays(MillisTime_t ms) {
    if (!ms) {
        return;
    }

    MillisTime_t startMillis = ERaMillis();
    do {
#if defined(ERA_NO_RTOS)
        if (this->runApiResponse) {
            this->thisApi().run();
            this->thisApi().runZigbee();
        }
#endif
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE) ||
            ModbusState::is(ModbusStateT::STATE_MB_CONTROLLED)) {
            break;
        }
        this->waitRequest();
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, ms));
}

template <class Api>
void ERaModbus<Api>::delaysMutex(MillisTime_t ms) {
    if (ms <= ERA_MODBUS_MUTEX_MS) {
        this->delays(ms);
        ERaGuardUnlock(this->mutex);
        return;
    }

    ms -= ERA_MODBUS_MUTEX_MS;
    this->delays(ERA_MODBUS_MUTEX_MS);
    ERaGuardUnlock(this->mutex);
    this->delays(ms);
}

template <class Api>
ModbusConfigAlias_t* ERaModbus<Api>::getModbusAlias(const char* key) {
    ERaList<ModbusConfigAlias_t*>::iterator* b = this->modbusConfig->modbusConfigAliasParam.begin();
    const ERaList<ModbusConfigAlias_t*>::iterator* e = this->modbusConfig->modbusConfigAliasParam.end();
    if (this->modbusControl->isJson) {
        b = this->modbusControl->modbusConfigAliasParam.begin();
        e = this->modbusControl->modbusConfigAliasParam.end();
    }
    for (ERaList<ModbusConfigAlias_t*>::iterator* it = b; it != e; it = it->getNext()) {
        if (it->get() == nullptr) {
            continue;
        }
        if (!strcmp(it->get()->key, key)) {
            return it->get();
        }
    }
    return nullptr;
}

template <class Api>
void ERaModbus<Api>::addScanData() {
    if (this->modbusScan == nullptr) {
        return;
    }

    if (this->modbusScan->numberDevice) {
        this->dataBuff.add(ERA_F(MODBUS_STRING_SCAN));
        this->dataBuff.add_hex_array(this->modbusScan->addr,
                            this->modbusScan->numberDevice);
    }
    else {
        this->dataBuff.add_multi(ERA_F(MODBUS_STRING_SCAN), ERA_F("None"));
    }
}

template <class Api>
void ERaModbus<Api>::processModbusScan() {
    if (!ModbusState::is(ModbusStateT::STATE_MB_SCAN)) {
        return;
    }
    if (this->modbusScan == nullptr) {
        return;
    }

    ModbusConfig_t param {
        .id = 0,
        .addr = this->modbusScan->start,
        .func = ModbusFunctionT::READ_HOLDING_REGISTERS,
        .sa1 = 0,
        .sa2 = 0,
        .len1 = 0,
        .len2 = 1,
        .extra = {},
        .delay = 20,
        .ipSlave = {
            .ip = {
                .dword = this->modbusScan->ipSlave.ip.dword
            },
            .port = this->modbusScan->ipSlave.port
        }
    };

    for (size_t i = this->modbusScan->start; (i < this->modbusScan->end) &&
        (this->modbusScan->numberDevice < this->modbusScan->numberScan); ++i) {
        param.addr = i;
        ERaGuardLock(this->mutex);
        this->nextTransport(param);
        if (ModbusTransp::readHoldingRegisters(this->transp, param, true)) {
            this->modbusScan->addr[this->modbusScan->numberDevice++] = i;
        }
        this->delays(100);
        ERaGuardUnlock(this->mutex);
        ERA_MODBUS_YIELD();
    }
}

#if defined(ERA_PNP_MODBUS)
    template <class Api>
    void ERaModbus<Api>::processModbusControl() {
        if (this->modbusControl->modbusConfigParam.isEmpty()) {
            return;
        }
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            return;
        }

        static bool wrote {true};
        if (!this->modbusControl->updated() && wrote) {
            return;
        }
        wrote = false;

        const ERaList<ModbusConfig_t*>::iterator* e = this->modbusControl->modbusConfigParam.end();
        for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusControl->modbusConfigParam.begin(); it != e; it = it->getNext()) {
            ModbusConfig_t* param = it->get();
            if (!this->handlerModbusWrite(param)) {
                return;
            }
        }

        wrote = true;
    }
#endif

template <class Api>
void ERaModbus<Api>::writeAllModbusWithOption(bool execute) {
    if (!this->modbusControl->writeOption.enable && !execute) {
        return;
    }
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        return;
    }

    const ERaList<ModbusConfig_t*>::iterator* e = this->modbusControl->modbusConfigParam.end();
    for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusControl->modbusConfigParam.begin(); it != e; it = it->getNext()) {
        ModbusConfig_t* param = it->get();
        if (param == nullptr) {
            continue;
        }
        param->len1 = this->modbusControl->writeOption.len1;
        param->len2 = this->modbusControl->writeOption.len2;
        memcpy(param->extra, this->modbusControl->writeOption.extra,
                            sizeof(param->extra));
        if (!this->handlerModbusWrite(param)) {
            return;
        }
    }

    this->modbusControl->writeOption.enable = false;
}

template <class Api>
bool ERaModbus<Api>::actionModbus(ModbusAction_t& request) {
    if (request.key == nullptr) {
        return false;
    }
    if (strlen(request.key) != 36) {
        return false;
    }

    ModbusConfigAlias_t* alias = this->getModbusAlias(request.key);
    if (alias == nullptr) {
        return false;
    }

    ModbusConfig_t* config = nullptr;
    for (ERaUInt_t i = 0; i < alias->readActionCount; ++i) {
        ERaGuardLock(this->mutex);
        this->eachActionModbus(request, alias->action[i], config);
        if (config == nullptr) {
            ERaGuardUnlock(this->mutex);
        }
        else if (alias->action[i].delay) {
            this->delaysMutex(alias->action[i].delay);
        }
        else if (i != (alias->readActionCount - 1)) {
            this->delayModbus(config->addr, true);
        }
        else {
            this->delayModbus(config->addr, true, true);
        }
    }
    
    return true;
}

template <class Api>
ModbusConfig_t* ERaModbus<Api>::getModbusConfig(ERaUInt_t id) {
    const ERaList<ModbusConfig_t*>::iterator* e = this->modbusControl->modbusConfigParam.end();
    for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusControl->modbusConfigParam.begin(); it != e; it = it->getNext()) {
        if (it->get() == nullptr) {
            continue;
        }
        if (it->get()->id == id) {
            return it->get();
        }
    }
    return nullptr;
}

template <class Api>
ModbusConfig_t* ERaModbus<Api>::getModbusConfigWithAddress(uint8_t addr) {
    const ERaList<ModbusConfig_t*>::iterator* e = this->modbusControl->modbusConfigParam.end();
    for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusControl->modbusConfigParam.begin(); it != e; it = it->getNext()) {
        if (it->get() == nullptr) {
            continue;
        }
        if (it->get()->addr == addr) {
            return it->get();
        }
    }
    return nullptr;
}

template <class Api>
bool ERaModbus<Api>::eachActionModbus(ModbusAction_t& request, Action_t& action, ModbusConfig_t*& config) {
    config = this->getModbusConfig(action.id);
    if (config == nullptr) {
        return false;
    }

    switch (request.type) {
        case ModbusActionTypeT::MODBUS_ACTION_DEFAULT:
            config->len1 = action.len1;
            config->len2 = action.len2;
            break;
        case ModbusActionTypeT::MODBUS_ACTION_PARAMS:
            config->len1 = HI_WORD(request.param);
            config->len2 = LO_WORD(request.param);
            break;
        default:
            break;
    }

    memcpy(config->extra, action.extra, sizeof(config->extra));

    if (this->pModbusCallbacks != nullptr) {
        this->pModbusCallbacks->onAction(&request, &action, config);
    }

    return this->sendModbusWrite(*config);
}

template <class Api>
bool ERaModbus<Api>::actionModbusRaw(ModbusActionRaw_t& request) {
    bool status {false};
    ModbusConfig_t config {};
    ModbusConfig_t* writeConfig = this->getModbusConfigWithAddress(request.addr);

    memcpy(&config.addr, &request.addr, 6);
    if (request.pExtra != nullptr) {
        memcpy(config.extra, request.pExtra, ERaMin(sizeof(config.extra), request.pExtraLen));
    }
    if (request.ip.ip.dword) {
        config.ipSlave.port = request.ip.port;
        config.ipSlave.ip.dword = request.ip.ip.dword;
    }
    else if (writeConfig != nullptr) {
        config.ipSlave.port = writeConfig->ipSlave.port;
        config.ipSlave.ip.dword = writeConfig->ipSlave.ip.dword;
    }

    if (request.prevdelay) {
        this->delays(request.prevdelay);
    }
    ERaGuardLock(this->mutex);
    status = this->sendModbusWrite(config);
    if (request.postdelay) {
        this->delays(request.postdelay);
        ERaGuardUnlock(this->mutex);
    }
    else {
        this->delayModbus(request.addr, true, true);
    }

    return status;
}

template <class Api>
void ERaModbus<Api>::writeAllModbus(uint8_t len1, uint8_t len2, const uint8_t* pData,
                                    size_t pDataLen, bool force, bool execute) {
    if ((this->modbusConfig == nullptr) ||
        (this->modbusControl == nullptr)) {
        return;
    }
    if (!this->modbusConfig->autoClosing && !force) {
        return;
    }

    this->modbusControl->writeOption.len1 = len1;
    this->modbusControl->writeOption.len2 = len2;
    if (pData != nullptr) {
        memcpy(this->modbusControl->writeOption.extra, pData,
        ERaMin(pDataLen, sizeof(this->modbusControl->writeOption.extra)));
    }

    if (execute) {
#if defined(ERA_NO_RTOS)
        this->runApiResponse = false;
#endif
        this->writeAllModbusWithOption(execute);
#if defined(ERA_NO_RTOS)
        this->runApiResponse = true;
#endif
    }
    else {
        this->modbusControl->writeOption.enable = true;
    }
}

template <class Api>
void ERaModbus<Api>::sendModbusRead(ModbusConfig_t& param) {
    bool skip {false};
    bool status {false};
    this->nextTransport(param);

    if (!this->failCounter.min) {
    }
    else if (param.totalFail >= this->failCounter.min) {
        skip = true;
    }

    switch (param.func) {
        case ModbusFunctionT::READ_COIL_STATUS:
            status = ModbusTransp::readCoilStatus(this->transp, param);
            break;
        case ModbusFunctionT::READ_INPUT_STATUS:
            status = ModbusTransp::readInputStatus(this->transp, param);
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
            status = ModbusTransp::readHoldingRegisters(this->transp, param);
            break;
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            status = ModbusTransp::readInputRegisters(this->transp, param);
            break;
        default:
            return;
    }

#if defined(ERA_NO_RTOS)
    ERaWatchdogFeed();
#endif

    if (skip) {
    }
    else if (!status) {
        this->failRead++;
    }
}

template <class Api>
bool ERaModbus<Api>::handlerModbusRead(ModbusConfig_t* param) {
    if (param == nullptr) {
        return false;
    }
    ERaGuardLock(this->mutex);
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE) ||
        ModbusState::is(ModbusStateT::STATE_MB_CONTROLLED)) {
        ERaGuardUnlock(this->mutex);
        this->executeNow();
        return false;
    }
    this->sendModbusRead(*param);
    this->delayModbus(param->addr, true);
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE) ||
        ModbusState::is(ModbusStateT::STATE_MB_CONTROLLED)) {
        this->executeNow();
        return false;
    }
    return true;
}

template <class Api>
bool ERaModbus<Api>::sendModbusWrite(ModbusConfig_t& param) {
    bool status {false};
    this->nextTransport(param);
    switch (param.func) {
        case ModbusFunctionT::FORCE_SINGLE_COIL:
            status = ModbusTransp::forceSingleCoil(this->transp, param);
            break;
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
            status = ModbusTransp::presetSingleRegister(this->transp, param);
            break;
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
            status = ModbusTransp::forceMultipleCoils(this->transp, param);
            break;
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
            status = ModbusTransp::presetMultipleRegisters(this->transp, param);
            break;
        default:
            return false;
    }

#if defined(ERA_NO_RTOS)
    ERaWatchdogFeed();
#endif

    if (!status) {
        this->failWrite++;
    }
    return status;
}

template <class Api>
bool ERaModbus<Api>::handlerModbusWrite(ModbusConfig_t* param) {
    if (param == nullptr) {
        return false;
    }
    ERaGuardLock(this->mutex);
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        ERaGuardUnlock(this->mutex);
        return false;
    }
    this->sendModbusWrite(*param);
    this->delayModbus(param->addr, true);
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        return false;
    }
    return true;
}

template <class Api>
void ERaModbus<Api>::onData(ERaModbusRequest* request, ERaModbusResponse* response, bool skip) {
    if (skip) {
        return;
    }
    if ((request == nullptr) ||
        (response == nullptr)) {
        return;
    }
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        return;
    }

    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS: {
            LOC_BUFFER_MODBUS(1)
            uint16_t bits = request->getLength();
            for (int i = 0; (i < response->getBytes()) && (bits > 0); ++i) {
                for (int j = 0; j < ((bits > 8) ? 8 : bits); ++j) {
                    pData[i * 8 + j] = this->getBit(response->getData()[i], j);
                }
                bits -= ((bits > 8) ? 8 : bits);
            }
            this->dataBuff.add_hex_array(pData, pDataLen);
            FREE_BUFFER_MODBUS
        }
            break;
        default:
            this->dataBuff.add_hex_array(response->getData(), response->getBytes());
            break;
    }
    if (response->getBytes()) {
        this->dataBuff.add_on_change("1");
    }

    if (this->pModbusCallbacks != nullptr) {
        this->pModbusCallbacks->onData(request, response,
                                response->getDataBuffer());
    }
}

template <class Api>
void ERaModbus<Api>::onError(ERaModbusRequest* request, bool skip) {
    if (skip) {
        return;
    }
    if (request == nullptr) {
        return;
    }
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        return;
    }

    size_t pDataLen {0};
    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            pDataLen = request->getLength();
            if (!this->dataBuff.next(pDataLen)) {
                this->dataBuff.add_zero_array(pDataLen);
            }
            break;
        default:
            pDataLen = (request->getLength() * 2);
            if (!this->dataBuff.next(pDataLen)) {
                this->dataBuff.add_zero_array(pDataLen);
            }
            break;
    }
    if (request->getLength()) {
        this->dataBuff.add_on_change("0");
    }

    if (this->pModbusCallbacks != nullptr) {
        this->pModbusCallbacks->onError(request);
    }
}

template <class Api>
void ERaModbus<Api>::processStateBegin(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data) {
    ModbusSlave::clear();
    if (response != nullptr) {
        response->clear();
    }

    ModbusSlave::setSlaveTransport(this->transp);

    if (this->isRTU()) {
        return this->processStateAddr(request, response, data);
    }
    /* Header Modbus TCP */
    uint8_t header[6] {0};

    header[0] = data;
    if (ModbusStream::readBytes(header + 1, 5) < 5) {
        ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_END;
        return;
    }

    for (size_t i = 0; i < sizeof(header); ++i) {
        ModbusSlave::add(header[i]);
        if (response == nullptr) {
            continue;
        }
        response->add(header[i]);
    }

    ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_ADDR;
}

template <class Api>
void ERaModbus<Api>::processStateAddr(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data) {
    ModbusSlave::add(data);
    ModbusSlave::setSlaveAddress(data);
    if (response != nullptr) {
        response->add(data);
    }

    ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_FUNC;

    if (request == nullptr) {
        return;
    }
    if (request->getSlaveAddress() == data) {
    }
    else if (response != nullptr) {
        response->clear();
    }
}

template <class Api>
void ERaModbus<Api>::processStateFunc(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data) {
    ModbusSlave::add(data);
    ModbusSlave::setSlaveFunction(data);

    if (response == nullptr) {
    }
    else if (!response->empty()) {
        response->add(data);
    }

    switch (data) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
            if ((request == nullptr) || (response == nullptr)) {
            }
            else if (request->getFunction() != data) {
                response->clear();
            }
            ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_DATA;
            return;
        default:
            if ((data & 0x80) != 0x80) {
                break;
            }
            ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_DATA;
            return;
    }

    ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_END;
}

template <class Api>
void ERaModbus<Api>::processStateData(ERaModbusRequest* request, ERaModbusResponse* response, uint8_t data) {
    bool complete {false};

    ModbusSlave::add(data);

    if ((response == nullptr) || response->empty()) {
        complete = ModbusSlave::isComplete();
    }
    else if (!response->empty()) {
        response->add(data);
        complete = response->isComplete();
    }

    if (complete) {
        ModbusSlave::handlerSlave();
        ModbusSlave::clear();
    }

    if (complete) {
        ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_END;
    }

    ERA_FORCE_UNUSED(request);
}

template <class Api>
void ERaModbus<Api>::processInput(ERaModbusRequest* request, ERaModbusResponse* response) {
    while (ModbusStream::availableBytes()) {
        int c = ModbusStream::readByte();
        if (c < 0) {
            continue;
        }
        uint8_t data = (uint8_t)c;
        switch (ModbusStream::state) {
            case ModbusStreamStateT::MB_STREAM_STATE_BEGIN:
            case ModbusStreamStateT::MB_STREAM_STATE_END:
                this->processStateBegin(request, response, data);
                break;
            case ModbusStreamStateT::MB_STREAM_STATE_ADDR:
                this->processStateAddr(request, response, data);
                break;
            case ModbusStreamStateT::MB_STREAM_STATE_FUNC:
                this->processStateFunc(request, response, data);
                break;
            case ModbusStreamStateT::MB_STREAM_STATE_DATA:
                this->processStateData(request, response, data);
                break;
            default:
                break;
        }
        if (ModbusStream::state == ModbusStreamStateT::MB_STREAM_STATE_END) {
            ModbusStream::state = ModbusStreamStateT::MB_STREAM_STATE_BEGIN;
            break;
        }
    }
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ERaModbusRequest* request, ERaModbusResponse* response) {
    if (request == nullptr) {
        return false;
    }
    if (response == nullptr) {
        return false;
    }

    MillisTime_t startMillis = ERaMillis();

    ModbusStream::streamStart();

    do {
        if (!ModbusStream::availableBytes()) {
#if defined(ERA_NO_RTOS)
            if (this->runApiResponse) {
                this->thisApi().run();
                this->thisApi().runZigbee();
            }
#endif
            if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                break;
            }
            ERA_MODBUS_YIELD();
            continue;
        }

        this->processInput(request, response);

        if (response->isComplete()) {
            ERaLogHex("MB <<", response->getMessage(), response->getPosition());
            return response->isSuccess();
        }
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, this->timeout));

    if (ModbusStream::state != ModbusStreamStateT::MB_STREAM_STATE_BEGIN) {
        this->waitRequest();
    }

    ModbusStream::streamEnd();

    return false;
}

template <class Api>
void ERaModbus<Api>::waitRequest() {
    if (!ModbusSlave::isEnabled()) {
        return;
    }
    this->processInput(nullptr, nullptr);
}

template <class Api>
void ERaModbus<Api>::sendRequest(const uint8_t* data, size_t size) {
    if (data == nullptr) {
        return;
    }

    ERaLogHex("MB >>", data, size);
    this->sendCommand(data, size);
}

template <class Api>
void ERaModbus<Api>::sendCommand(const uint8_t* data, size_t size) {
    this->switchToTransmit();
    ModbusStream::sendBytes(data, size);
    ModbusStream::flushBytes();
    this->switchToReceive();
}

template <class Api>
void ERaModbus<Api>::setModbusDEPin(int _pin) {
    if (_pin < 0) {
        return;
    }

    this->dePin = _pin;
#if defined(ARDUINO) ||      \
    defined(RASPBERRY) ||    \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI)
    pinMode(this->dePin, OUTPUT);
    ::digitalWrite(this->dePin, LOW);
#endif
}

template <class Api>
void ERaModbus<Api>::switchToTransmit() {
    if (this->dePin < 0) {
        return;
    }

#if defined(ARDUINO) ||      \
    defined(RASPBERRY) ||    \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI)
    ::digitalWrite(this->dePin, HIGH);
    if (this->predelay) {
        ERaDelayUs(this->predelay);
    }
#endif
}

template <class Api>
void ERaModbus<Api>::switchToReceive() {
    if (this->dePin < 0) {
        return;
    }

#if defined(ARDUINO) ||      \
    defined(RASPBERRY) ||    \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI)
    if (this->postdelay) {
        ERaDelayUs(this->postdelay);
    }
    ::digitalWrite(this->dePin, LOW);
#endif
}

template <class S, typename... Args>
class SerialModBus {
public:
    static S& serial(Args... tail) {
        static S _serial(tail...);
        return _serial;
    }
};

#endif /* INC_ERA_MODBUS_HPP_ */
