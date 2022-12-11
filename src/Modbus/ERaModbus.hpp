#ifndef INC_ERA_MODBUS_HPP_
#define INC_ERA_MODBUS_HPP_

#include <vector>
#include <cmath>
#include <ERa/ERaParam.hpp>
#include <Utility/ERaUtility.hpp>
#include <Modbus/ERaModbusState.hpp>
#include <Modbus/ERaParse.hpp>
#include "ERaModbusConfig.hpp"
#include "ERaDefineModbus.hpp"

using namespace std;

ERaApplication ERaApplication::config {};
ERaApplication ERaApplication::control {};

template <class Api>
class ERaModbus
{
    enum ModbusFunctionT {
        READ_COIL_STATUS = 0x01,
        READ_INPUT_STATUS = 0x02,
        READ_HOLDING_REGISTERS = 0x03,
        READ_INPUT_REGISTERS = 0x04,
        FORCE_SINGLE_COIL = 0x05,
        PRESET_SINGLE_REGISTER = 0x06,
        FORCE_MULTIPLE_COILS = 0x0F,
        PRESET_MULTIPLE_REGISTERS = 0x10
    };
    typedef struct __ModbusAction_t {
        char* key;
    } ModbusAction_t;
    typedef void* TaskHandle_t;
    typedef void* QueueMessage_t;
    const char* TAG = "Modbus";
    const char* FILENAME_CONFIG = FILENAME_MODBUS_CONFIG;
    const char* FILENAME_CONTROL = FILENAME_MODBUS_CONTROL;

public:
    ERaModbus()
        : total(0)
        , failRead(0)
        , failWrite(0)
#if defined(ARDUINO)
        , stream(NULL)
#elif defined(LINUX)
        , fd(-1)
#endif
        , _streamDefault(false)
        , _modbusTask(NULL)
        , _writeModbusTask(NULL)
        , messageHandle(NULL)
        , mutex(NULL)
    {}
    ~ERaModbus()
    {}

#if defined(ARDUINO)
    void setModbusStream(Stream& _stream) {
        this->stream = &_stream;
    }
#elif defined(LINUX)
    void setModbusStream(int _fd) {
        this->fd = _fd;
    }
#endif

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
    bool waitResponse(ModbusConfig_t& param, uint8_t* modbusData);
    void processData(ModbusConfig_t& param, uint8_t* modbusData);
    bool checkReceiveCRC(ModbusConfig_t& param, uint8_t* data);
    void sendCommand(const vector<uint8_t>& data);
    void modbusCRC(vector<uint8_t>& cmd);

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
#if defined(ARDUINO)
    Stream* stream;
#elif defined(LINUX)
    int fd;
#endif
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
    uint16_t len = param.len1 << 8 | param.len2;
    uint16_t lenByte {0};
    switch (param.func) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            lenByte = ceil(static_cast<float>(len)/8);
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            lenByte = len * 2;
            break;
        default:
            return;
    }
    
    uint8_t modbusData[256] {0};
    vector<uint8_t> command;
    command.assign(reinterpret_cast<uint8_t*>(&param.addr), reinterpret_cast<uint8_t*>(&param.addr) + 6);
    this->modbusCRC(command);
    this->sendCommand(command);
    if (this->waitResponse(param, modbusData)) {
        switch (param.func) {
            case ModbusFunctionT::READ_COIL_STATUS:
            case ModbusFunctionT::READ_INPUT_STATUS: {
                uint8_t lenBit = len;
                uint8_t pData[len + 1] {0};
                for (int i = 0; i < lenByte; ++i) {
                    for (int j = 0; j < ((lenBit > 8) ? 8 : lenBit); ++j) {
                        pData[i * 8 + j] = ((modbusData[i + 3] >> j) & 0x01);
                    }
                    lenBit -= (lenBit > 8) ? 8 : lenBit;
                }
                this->dataBuff.add_hex_array(pData, len);
            }
                break;
            default:
                this->dataBuff.add_hex_array(modbusData + 3, lenByte);
                break;
        }
        param.totalFail = 0;
        this->dataBuff.add("1");
    }
    else {
        if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
            return;
        }
        switch (param.func) {
            case ModbusFunctionT::READ_COIL_STATUS:
            case ModbusFunctionT::READ_INPUT_STATUS:
                if (!this->dataBuff.next()) {
                    uint8_t pData[len + 1] {0};
                    this->dataBuff.add_hex_array(pData, len);
                }
                break;
            default:
                if (!this->dataBuff.next()) {
                    uint8_t pData[lenByte + 1] {0};
                    this->dataBuff.add_hex_array(pData, lenByte);
                }
                break;
        }
        this->failRead++;
        param.totalFail++;
        this->dataBuff.add("0");
    }
}

template <class Api>
bool ERaModbus<Api>::sendModbusWrite(ModbusConfig_t& param) {
    uint16_t lenByte = param.len1 << 8 | param.len2;

    uint8_t modbusData[256] {0};
    vector<uint8_t> command;
    command.assign(reinterpret_cast<uint8_t*>(&param.addr), reinterpret_cast<uint8_t*>(&param.addr) + 6);
    
    switch (param.func) {
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
            break;
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
            lenByte = std::ceil(static_cast<float>(lenByte) / 8);
            command.push_back(lenByte & 0xFF);
            for(int i = 0; i < lenByte; ++i) {
                command.push_back(param.extra[i]);
            }
            break;
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
            lenByte *= 2;
            command.push_back(lenByte & 0xFF);
            for(int i = 0; i < lenByte; ++i) {
                command.push_back(param.extra[i]);
            }
            break;
        default:
            return false;
    }

    this->modbusCRC(command);

    for (size_t i = 0; i < 2; ++i) {
        this->sendCommand(command);
        if (this->waitResponse(param, modbusData)) {
            return true;
        }
    }

    this->failWrite++;
    return false;
}

template <class Api>
void ERaModbus<Api>::processData(ModbusConfig_t& param, uint8_t* modbusData) {

}

template <class Api>
bool ERaModbus<Api>::checkReceiveCRC(ModbusConfig_t& param, uint8_t* data) {
    uint8_t len{0};
    uint16_t lenByte = param.len1 << 8 | param.len2;
    switch (param.func) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            len = 3 + std::ceil(static_cast<float>(lenByte) / 8);
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            len = 3 + 2 * lenByte;
            break;
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
            len = 6;
            break;
        default:
            return false;
    }
    if (len < 4) {
        return false;
    }
    vector<uint8_t> command;
    command.assign(data, data + len);
    modbusCRC(command);
    if (data[len] == command.at(len)) {
        if (data[len + 1] == command.at(len + 1)) {
            return true;
        }
    }
    return false;
}

template <class Api>
void ERaModbus<Api>::modbusCRC(vector<uint8_t>& cmd) {
    uint16_t crc = 0xFFFF;

    for (size_t pos = 0; pos < cmd.size(); pos++) {
        crc ^= (uint16_t)cmd.at(pos);

        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else {
                crc >>= 1;
            }
        }
    }
    
    cmd.push_back(crc & 0xFF);
    cmd.push_back(crc >> 8);
}

#endif /* INC_ERA_MODBUS_HPP_ */
