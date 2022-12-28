#ifndef INC_ERA_MODBUS_HPP_
#define INC_ERA_MODBUS_HPP_

#include <vector>
#include <cmath>
#include <ERa/ERaParam.hpp>
#include <Utility/ERaUtility.hpp>
#include <Modbus/ERaModbusState.hpp>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaModbusConfig.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusTransp.hpp>

using namespace std;

ERaApplication ERaApplication::config {};
ERaApplication ERaApplication::control {};

template <class Api>
class ERaModbus
    : public ERaModbusTransp < ERaModbus<Api> >
{
    typedef struct __ModbusAction_t {
        char* key;
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

    friend class ERaModbusTransp < ERaModbus<Api> >;
	typedef ERaModbusTransp < ERaModbus<Api> > ModbusTransp;

public:
    ERaModbus()
        : total(0)
        , failRead(0)
        , failWrite(0)
        , dePin(-1)
        , transp(0)
        , client(NULL)
        , ip {
            .ip = {},
            .port = 502
        }
        , stream(NULL)
        , _streamDefault(false)
        , _modbusTask(NULL)
        , _writeModbusTask(NULL)
        , messageHandle(NULL)
        , mutex(NULL)
    {}
    ~ERaModbus()
    {
        this->switchToModbusRTU();
    }

    void setModbusClient(Client& _client, IPAddress _ip, uint16_t _port = 502) {
        this->client = &_client;
        this->ip.ip = _ip;
        this->ip.port = _port;
        this->setModbusStream(_client);
    }

    void setModbusStream(Stream& _stream) {
        this->stream = &_stream;
    }

    void switchToModbusRTU() {
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_RTU;
    }

    void switchToModbusTCP() {
        this->transp = ModbusTransportT::MODBUS_TRANSPORT_TCP;
    }

    void setModbusDEPin(int _pin);

protected:
    void begin() {
        this->dataBuff.allocate(MODBUS_DATA_BUFFER_SIZE);
        this->configModbus();
        this->initModbusConfig();
        this->initModbusTask();
    }

    void run(const bool isRead) {
        for (;;) {
            if (isRead) {
                this->runRead();
            } else {
                this->runWrite();
            }
            ERA_MODBUS_YIELD();
        }
    }

    void runRead() {
        ModbusState::set(ModbusStateT::STATE_MB_RUNNING);
        if (!ERaRemainingTime(this->modbusConfig.modbusInterval.prevMillis, this->modbusConfig.modbusInterval.delay)) {
            this->modbusConfig.modbusInterval.prevMillis = ERaMillis();
            this->readModbusConfig();
        }
    }

    void runWrite() {
        if (!this->isRequest()) {
            return;
        }
        ModbusAction_t& req = this->getRequest();
        if (req.key == nullptr) {
            return;
        }
        this->actionModbus(req.key);
        if (this->isEmptyRequest()) {
            this->executeNow();
            ModbusState::set(ModbusStateT::STATE_MB_CONTROLLED);
        }
        free(req.key);
        req.key = nullptr;
    }

    void parseModbusConfig(char* ptr, bool isControl = false) {
        if (isControl) {
            this->modbusControl.parseConfig(ptr);
            this->thisApi().writeToFlash(FILENAME_CONTROL, ptr);
        } else {
            this->modbusConfig.parseConfig(ptr);
            this->setBaudRate(this->modbusConfig.baudSpeed);
            this->executeNow();
            this->thisApi().writeToFlash(FILENAME_CONFIG, ptr);
        }
        ModbusState::set(ModbusStateT::STATE_MB_PARSE);
    }

    void removeConfigFromFlash() {
        this->modbusConfig.deleteAll();
        this->modbusControl.deleteAll();
        this->thisApi().removeFromFlash(FILENAME_CONFIG);
        this->thisApi().removeFromFlash(FILENAME_CONTROL);
    }

    void clearDataBuff() {
        this->dataBuff.clearBuffer();
    }

    bool addModbusAction(const char* ptr) {
        if (strlen(ptr) != 36) {
            return false;
        }
	    if (!this->queue.writeable()) {
            return false;
        }
        ModbusAction_t req;
        req.key = (char*)ERA_MALLOC(37 * sizeof(char));
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
    void initModbusConfig() {
        char* ptr = nullptr;
        ptr = this->thisApi().readFromFlash(FILENAME_CONFIG);
        this->modbusConfig.parseConfig(ptr);
        this->setBaudRate(this->modbusConfig.baudSpeed);
        free(ptr);
        ptr = this->thisApi().readFromFlash(FILENAME_CONTROL);
        this->modbusControl.parseConfig(ptr);
        free(ptr);
        ptr = nullptr;
    }

    bool connectTCPIp() {
        if (this->client == nullptr) {
            return true;
        }
        if (!this->client->connected()) {
            return this->client->connect(ip.ip, ip.port);
        }
        return true;
    }

    void configModbus();
    void setBaudRate(uint32_t baudrate);
    void readModbusConfig();
    void delayModbus(const int address);
    ModbusConfigAlias_t* getModbusAlias(const char* key);
    ModbusConfig_t* getModbusConfig(int id);
    bool actionModbus(const char* key);
    bool eachActionModbus(Action_t& action, ModbusConfig_t*& config);
    void sendModbusRead(ModbusConfig_t& param);
    bool sendModbusWrite(ModbusConfig_t& param);
    void onData(ERaModbusRequest* request, ERaModbusResponse* response);
    void onError(ERaModbusRequest* request);
    bool waitResponse(ERaModbusResponse* response);
    void sendCommand(uint8_t* data, size_t size);
    void switchToTransmit();
    void switchToReceive();

    bool getBit(uint8_t byte, uint8_t pos) {
        return ((byte >> pos) & 0x01);
    }

    bool streamDefault() {
        return this->_streamDefault;
    }

    void executeNow() {
        this->modbusConfig.modbusInterval.prevMillis = ERaMillis() - this->modbusConfig.modbusInterval.delay + ERA_MODBUS_EXECUTE_MS;
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

	ERaQueue<ModbusAction_t, 10> queue;
    ERaDataBuffDynamic dataBuff;
    ERaApplication& modbusConfig = ERaApplication::config;
    ERaApplication& modbusControl = ERaApplication::control;
    int total;
    int failRead;
    int failWrite;
    int dePin;
    uint8_t transp;
    Client* client;
    TCPIp_t ip;
    Stream* stream;
    bool _streamDefault;
    TaskHandle_t _modbusTask;
    TaskHandle_t _writeModbusTask;
    QueueMessage_t messageHandle;
    ERaMutex_t mutex;
};

template <class Api>
void ERaModbus<Api>::readModbusConfig() {
    if (this->modbusConfig.modbusConfigParam.isEmpty()) {
        return;
    }
    if (!this->connectTCPIp()) {
        return;
    }
    this->dataBuff.clear();
    for (size_t i = 0; i < this->modbusConfig.readConfigCount; ++i) {
        ModbusConfig_t& param = *this->modbusConfig.modbusConfigParam[i];
        ERaGuardLock(this->mutex);
        this->sendModbusRead(param);
        ERaGuardUnlock(this->mutex);
        this->delayModbus(param.addr);
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE) ||
            ModbusState::is(ModbusStateT::STATE_MB_CONTROLLED)) {
            this->executeNow();
            return;
        }
    }
    this->dataBuff.add_multi(ERA_F("fail_read"), this->failRead, ERA_F("fail_write"), this->failWrite, ERA_F("total"), this->total);
    this->thisApi().modbusDataWrite(&this->dataBuff);
}

template <class Api>
void ERaModbus<Api>::delayModbus(const int address) {
    MillisTime_t delayMs {10L};
    MillisTime_t startMillis = ERaMillis();
    for (ERaList<SensorDelay_t>::iterator* it = this->modbusConfig.sensorDelay.begin(); it != nullptr; it = it->getNext()) {
        if (it->get().address == address) {
            delayMs = it->get().delay;
            break;
        }
    }
    do {
#if defined(ERA_NO_RTOS)
        ERaOnWaiting();
        this->thisApi().run();
#endif
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE) ||
            ModbusState::is(ModbusStateT::STATE_MB_CONTROLLED)) {
            break;
        }
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, delayMs));
}

template <class Api>
ModbusConfigAlias_t* ERaModbus<Api>::getModbusAlias(const char* key) {
    for (ERaList<ModbusConfigAlias_t>::iterator* it = this->modbusConfig.modbusConfigAliasParam.begin(); it != nullptr; it = it->getNext()) {
        if (!strcmp(it->get().key, key)) {
            return &it->get();
        }
    }
    return nullptr;
}

template <class Api>
bool ERaModbus<Api>::actionModbus(const char* key) {
    if (key == nullptr) {
        return false;
    }
    if (strlen(key) != 36) {
        return false;
    }

    ModbusConfigAlias_t* alias = this->getModbusAlias(key);
    if (alias == nullptr) {
        return false;
    }

    if (!this->connectTCPIp()) {
        return false;
    }

    ModbusConfig_t* config = nullptr;
    for (int i = 0; i < alias->readActionCount; ++i) {
        ERaGuardLock(this->mutex);
        eachActionModbus(alias->action[i], config);
        ERaGuardUnlock(this->mutex);
        if ((config != nullptr) &&
            (i != (alias->readActionCount - 1))) {
            this->delayModbus(config->addr);
        }
    }
    
    return true;
}

template <class Api>
ModbusConfig_t* ERaModbus<Api>::getModbusConfig(int id) {
    for (ERaList<ModbusConfig_t>::iterator* it = this->modbusControl.modbusConfigParam.begin(); it != nullptr; it = it->getNext()) {
        if (it->get().id == id) {
            return &it->get();
        }
    }
    return nullptr;
}

template <class Api>
bool ERaModbus<Api>::eachActionModbus(Action_t& action, ModbusConfig_t*& config) {
    config = this->getModbusConfig(action.id);
    if (config == nullptr) {
        return false;
    }

    config->len1 = action.len1;
    config->len2 = action.len2;

    memcpy(config->extra, action.extra, sizeof(config->extra));

    return this->sendModbusWrite(*config);
}

template <class Api>
void ERaModbus<Api>::sendModbusRead(ModbusConfig_t& param) {
    bool status {false};
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

    if (status) {
        param.totalFail = 0;
    }
    else {
        this->failRead++;
        param.totalFail++;
    }
}

template <class Api>
bool ERaModbus<Api>::sendModbusWrite(ModbusConfig_t& param) {
    bool status {false};
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

    if (!status) {
        this->failWrite++;
    }
    return status;
}

template <class Api>
void ERaModbus<Api>::onData(ERaModbusRequest* request, ERaModbusResponse* response) {
    if ((request == nullptr) ||
        (response == nullptr)) {
        return;
    }

    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS: {
            uint16_t bits = request->getLength();
            uint8_t pData[request->getLength()] {0};
            for (int i = 0; (i < response->getBytes()) && (bits > 0); ++i) {
                for (int j = 0; j < ((bits > 8) ? 8 : bits); ++j) {
                    pData[i * 8 + j] = this->getBit(response->getData()[i], j);
                }
                bits -= ((bits > 8) ? 8 : bits);
            }
            this->dataBuff.add_hex_array(pData, request->getLength());
        }
            break;
        default:
            this->dataBuff.add_hex_array(response->getData(), response->getBytes());
            break;
    }
    this->dataBuff.add("1");
}

template <class Api>
void ERaModbus<Api>::onError(ERaModbusRequest* request) {
    if (request == nullptr) {
        return;
    }
    if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
        return;
    }

    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            if (!this->dataBuff.next()) {
                uint8_t pData[request->getLength()] {0};
                this->dataBuff.add_hex_array(pData, request->getLength());
            }
            break;
        default:
            if (!this->dataBuff.next()) {
                uint8_t pData[request->getLength() * 2] {0};
                this->dataBuff.add_hex_array(pData, request->getLength() * 2);
            }
            break;
    }
    this->dataBuff.add("0");
}

template <class Api>
void ERaModbus<Api>::setModbusDEPin(int _pin) {
    if (_pin < 0) {
        return;
    }
    this->dePin = _pin;
#if defined(ARDUINO) || \
    (defined(LINUX) &&  \
    defined(RASPBERRY))
    pinMode(this->dePin, OUTPUT);
#endif
}

template <class Api>
void ERaModbus<Api>::switchToTransmit() {
    if (this->dePin < 0) {
        return;
    }
#if defined(ARDUINO) || \
    (defined(LINUX) &&  \
    defined(RASPBERRY))
    ::digitalWrite(this->dePin, HIGH);
#endif
}

template <class Api>
void ERaModbus<Api>::switchToReceive() {
    if (this->dePin < 0) {
        return;
    }
#if defined(ARDUINO) || \
    (defined(LINUX) &&  \
    defined(RASPBERRY))
    ::digitalWrite(this->dePin, LOW);
#endif
}

#endif /* INC_ERA_MODBUS_HPP_ */
