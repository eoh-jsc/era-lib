#ifndef INC_ERA_MODBUS_HPP_
#define INC_ERA_MODBUS_HPP_

#include <math.h>
#include <ERa/ERaParam.hpp>
#include <Utility/ERaUtility.hpp>
#include <Modbus/ERaModbusState.hpp>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaModbusConfig.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusTransp.hpp>
#include <Modbus/ERaModbusCallbacks.hpp>

using namespace std;

template <class Api>
class ERaModbus
    : public ERaModbusTransp < ERaModbus<Api> >
{
    typedef struct __ModbusAction_t {
        char* key;
        uint8_t type;
        uint16_t param;
    } ModbusAction_t;
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

    friend class ERaModbusTransp < ERaModbus<Api> >;
    typedef ERaModbusTransp < ERaModbus<Api> > ModbusTransp;

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
        , total(0)
        , failRead(0)
        , failWrite(0)
        , dePin(-1)
        , transp(0)
        , streamRTU(NULL)
        , clientTCP(NULL)
        , ip {
            .ip = {},
            .port = 502
        }
        , stream(NULL)
        , _streamDefault(false)
#if !defined(ERA_NO_RTOS)
        , _modbusTask(NULL)
        , _writeModbusTask(NULL)
#endif
        , messageHandle(NULL)
        , mutex(NULL)
        , skipModbus(false)
        , wifiConfig(false)
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
    }

    void setModbusTimeout(uint32_t _timeout) {
        this->timeout = _timeout;
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

    void setSkipModbus(bool skip) {
        this->skipModbus = skip;
    }

    void setConnectWiFiModbus(bool enable) {
        this->wifiConfig = enable;
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
            this->getModbusAction();
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

    void parseModbusConfig(char* config, const char* hash, const char* buf, bool isControl = false) {
        ModbusState::set(ModbusStateT::STATE_MB_PARSE);
        if (isControl) {
            ERaGuardLock(this->mutex);
            this->modbusControl->parseConfig(config);
            if (this->modbusControl->updateHashID(hash)) {
                this->thisApi().writeToFlash(FILENAME_CONTROL, buf);
            }
            ModbusState::set(ModbusStateT::STATE_MB_PARSE);
            ERaGuardUnlock(this->mutex);
            this->initModbus();
        }
        else {
            ERaGuardLock(this->mutex);
            this->modbusConfig->parseConfig(config);
            if (this->modbusConfig->updateHashID(hash)) {
                this->clearDataBuff();
                this->thisApi().writeToFlash(FILENAME_CONFIG, buf);
                if (this->wifiConfig && this->modbusConfig->isWiFi) {
                    this->thisApi().connectNewWiFi(this->modbusConfig->ssid,
                                                   this->modbusConfig->pass);
                }
            }
            ModbusState::set(ModbusStateT::STATE_MB_PARSE);
            ERaGuardUnlock(this->mutex);
            this->initModbus();
            if (this->initialized) {
                this->setBaudRate(this->modbusConfig->baudSpeed);
            }
            this->executeNow();
        }
    }

    void parseModbusScan(char* config) {
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
        this->thisApi().removeFromFlash(FILENAME_CONFIG);
        this->thisApi().removeFromFlash(FILENAME_CONTROL);
    }

    void clearDataBuff() {
        this->dataBuff.clearBuffer();
    }

    bool addModbusAction(const char* ptr) {
        return this->addModbusAction(ptr, ModbusActionTypeT::MODBUS_ACTION_DEFAULT, 0);
    }

    bool addModbusAction(const char* ptr, uint16_t param) {
        return this->addModbusAction(ptr, ModbusActionTypeT::MODBUS_ACTION_PARAMS, param);
    }

    bool addModbusAction(const char* ptr, uint8_t type, uint16_t param) {
        if (strlen(ptr) != 36) {
            return false;
        }
        if (!this->queue.writeable()) {
            return false;
        }
        char* buf = (char*)ERA_MALLOC(37 * sizeof(char));
        if (buf == nullptr) {
            return false;
        }
        ModbusAction_t req;
        req.key = buf;
        req.type = type;
        req.param = param;
        memset(req.key, 0, 37 * sizeof(char));
        strcpy(req.key, ptr);
        this->queue += req;
        return true;
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
                this->endModbus();
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

        this->configModbus();
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
        this->updateConfig(this->modbusConfig, ptr, "configuration");
        if (this->initialized) {
            this->setBaudRate(this->modbusConfig->baudSpeed);
        }
        free(ptr);
        ptr = this->thisApi().readFromFlash(FILENAME_CONTROL);
        this->updateConfig(this->modbusControl, ptr, "control");
        free(ptr);
        ptr = nullptr;
    }

    void updateConfig(ERaModbusEntry* config, const char* buf, const char* name) {
        cJSON* root = cJSON_Parse(buf);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }

        cJSON* data = cJSON_GetObjectItem(root, "data");
        cJSON* item = cJSON_GetObjectItem(data, name);
        if (cJSON_IsString(item)) {
            config->parseConfig(item->valuestring);
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
        }
        ModbusState::set(ModbusStateT::STATE_MB_RUNNING);
    }

    void nextTransport(const ModbusConfig_t& param) {
        this->switchToModbusRTU();
        if (this->clientTCP == nullptr) {
            return;
        }
        if (param.ipSlave.ip.dword) {
            this->switchToModbusTCP();
        }
        if ((ip.ip == IPAddress(param.ipSlave.ip.dword)) ||
            (param.ipSlave.ip.dword == 0)) {
            return;
        }
        if (param.ipSlave.port == 0) {
            return;
        }
        ip.ip = param.ipSlave.ip.dword;
        ip.port = param.ipSlave.port;
        // TODO
        if (this->clientTCP->connected()) {
            this->clientTCP->stop();
        }
        this->clientTCP->connect(ip.ip, ip.port);
    }

    void switchToModbusRTU() {
        this->stream = this->streamRTU;
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_RTU;
    }

    void switchToModbusTCP() {
        this->stream = this->clientTCP;
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_TCP;
    }

    void getModbusAction() {
        if (!this->isRequest()) {
            return;
        }
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            return;
        }
        ModbusAction_t& req = this->getRequest();
        if (req.key == nullptr) {
            return;
        }
        this->actionModbus(req);
        if (this->isEmptyRequest()) {
            this->executeNow();
            if (!ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                ModbusState::set(ModbusStateT::STATE_MB_CONTROLLED);
            }
        }
        free(req.key);
        req.key = nullptr;
    }

    void configModbus();
    void endModbus();
    void setBaudRate(uint32_t baudrate);
    void readModbusConfig();
    bool checkPubDataInterval();
    void delayModbus(const int address, bool unlock = false, bool skip = false);
    void delays(MillisTime_t ms);
    ModbusConfigAlias_t* getModbusAlias(const char* key);
    ModbusConfig_t* getModbusConfig(int id);
    void addScanData();
    void processModbusScan();
#if defined(ERA_PNP_MODBUS)
    void processModbusControl();
#endif
    void writeAllModbusWithOption(bool execute = false);
    bool actionModbus(ModbusAction_t& request);
    bool eachActionModbus(ModbusAction_t& request, Action_t& action, ModbusConfig_t*& config);
    void sendModbusRead(ModbusConfig_t& param);
    bool handlerModbusRead(ModbusConfig_t* param);
    bool sendModbusWrite(ModbusConfig_t& param);
    bool handlerModbusWrite(ModbusConfig_t* param);
    void onData(ERaModbusRequest* request, ERaModbusResponse* response, bool skip = false);
    void onError(ERaModbusRequest* request, bool skip = false);
    bool waitResponse(ERaModbusResponse* response);
    void sendCommand(uint8_t* data, size_t size);
    void switchToTransmit();
    void switchToReceive();

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

    void updateTotalTransmit() {
        if (this->total++ > 99) {
            this->total = 1;
            this->failRead = 0;
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
        do {
            ERaDelay(1000);
        } while (!this->thisApi().connected());
    }

    bool getBit(uint8_t byte, uint8_t pos) {
        return ((byte >> pos) & 0x01);
    }

    bool streamDefault() const {
        return this->_streamDefault;
    }

    void executeNow() {
        this->modbusConfig->modbusInterval.prevMillis = (ERaMillis() - this->modbusConfig->modbusInterval.delay + ERA_MODBUS_EXECUTE_MS);
    }

    bool isRequest() {
        return this->queue.readable();
    }

    ModbusAction_t& getRequest() {
        return this->queue;
    }

    bool isEmptyRequest() {
        return this->queue.isEmpty();
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

    ERaQueue<ModbusAction_t, MODBUS_MAX_ACTION> queue;
    ERaDataBuffDynamic dataBuff;
    ERaModbusEntry*& modbusConfig;
    ERaModbusEntry*& modbusControl;
    ERaScanEntry*& modbusScan;
    ERaModbusCallbacks* pModbusCallbacks;
    uint32_t timeout;
    unsigned long prevMillis;
    int total;
    int failRead;
    int failWrite;
    int dePin;
    uint8_t transp;
    Stream* streamRTU;
    Client* clientTCP;
    TCPIp_t ip;
    Stream* stream;
    bool _streamDefault;

#if !defined(ERA_NO_RTOS)
    TaskHandle_t _modbusTask;
    TaskHandle_t _writeModbusTask;
#endif

    QueueMessage_t messageHandle;
    ERaMutex_t mutex;
    bool skipModbus;
    bool wifiConfig;
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
                            ERA_F(MODBUS_STRING_TOTAL), this->total);
    this->addScanData();
    this->dataBuff.done();
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
void ERaModbus<Api>::delayModbus(const int address, bool unlock, bool skip) {
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
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, ms));
}

template <class Api>
ModbusConfigAlias_t* ERaModbus<Api>::getModbusAlias(const char* key) {
    const ERaList<ModbusConfigAlias_t*>::iterator* e = this->modbusConfig->modbusConfigAliasParam.end();
    for (ERaList<ModbusConfigAlias_t*>::iterator* it = this->modbusConfig->modbusConfigAliasParam.begin(); it != e; it = it->getNext()) {
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
        .len2 = 1
    };

    for (size_t i = this->modbusScan->start; (i < this->modbusScan->end) &&
        (this->modbusScan->numberDevice < this->modbusScan->numberScan); ++i) {
        param.addr = i;
        ERaGuardLock(this->mutex);
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

        static bool writed {true};
        if (!this->modbusControl->updated() && writed) {
            return;
        }
        writed = false;

        const ERaList<ModbusConfig_t*>::iterator* e = this->modbusControl->modbusConfigParam.end();
        for (ERaList<ModbusConfig_t*>::iterator* it = this->modbusControl->modbusConfigParam.begin(); it != e; it = it->getNext()) {
            ModbusConfig_t* param = it->get();
            if (!this->handlerModbusWrite(param)) {
                return;
            }
        }

        writed = true;
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
    for (int i = 0; i < alias->readActionCount; ++i) {
        ERaGuardLock(this->mutex);
        this->eachActionModbus(request, alias->action[i], config);
        if ((config != nullptr) &&
            (i != (alias->readActionCount - 1))) {
            this->delayModbus(config->addr, true);
        }
        else {
            this->delayModbus(config->addr, true, true);
        }
    }
    
    return true;
}

template <class Api>
ModbusConfig_t* ERaModbus<Api>::getModbusConfig(int id) {
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

    return this->sendModbusWrite(*config);
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
    bool status {false};
    this->nextTransport(param);
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

    if (status) {
        param.totalFail = 0;
    }
    else {
        this->failRead++;
        param.totalFail++;
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
