#ifndef INC_ERA_PARSE_HPP_
#define INC_ERA_PARSE_HPP_

#include <string.h>
#include <ERa/ERaDebug.hpp>
#include <ERa/ERaHelpersDef.hpp>
#include <Utility/ERaQueue.hpp>

#ifndef MAX_DEVICE_MODBUS
    #define MAX_DEVICE_MODBUS           20
#endif

#ifndef DEFAULT_MODBUS_BAUD_SPEED
    #define DEFAULT_MODBUS_BAUD_SPEED   9600
#endif

#ifndef DEFAULT_MODBUS_INTERVAL
    #define DEFAULT_MODBUS_INTERVAL     1000
#endif

#ifndef DEFAULT_PUB_MODBUS_INTERVAL
    #define DEFAULT_PUB_MODBUS_INTERVAL 60000
#endif

#ifndef DEFAULT_MIN_MODBUS_INTERVAL
    #define DEFAULT_MIN_MODBUS_INTERVAL 1000
#endif

typedef struct __SensorDelay_t {
    int delay;
    int address;
} SensorDelay_t;

typedef struct __IPSlave_t {
    union {
        uint8_t bytes[4];
        uint32_t dword;
    } ip;
    uint16_t port;
} IPSlave_t;

typedef struct __ModbusConfig_t {
    int id;
    uint8_t addr;
    uint8_t func;
    uint8_t sa1;
    uint8_t sa2;
    uint8_t len1;
    uint8_t len2;
    uint8_t extra[10];
    uint8_t type;
    uint8_t button;
    uint16_t delay;
    IPSlave_t ipSlave;
    uint8_t totalFail;
    uint8_t sizeData;
    uint32_t value;
    uint8_t ack;
} ModbusConfig_t;

typedef struct __Action_t {
    int id;
    uint8_t len1;
    uint8_t len2;
    uint8_t extra[10];
} Action_t;

typedef struct __ModbusConfigAlias_t {
    int readActionCount;
    int id;
    char key[37];
    Action_t action[10];
    uint16_t timer;
} ModbusConfigAlias_t;

typedef struct __ModbusWriteOption_t {
    bool enable;
    uint8_t len1;
    uint8_t len2;
    uint8_t extra[10];
} ModbusWriteOption_t;

typedef struct __IntervalDelay_t {
    uint32_t delay;
    unsigned long prevMillis;
} IntervalDelay_t;

class ERaModbusEntry
{
    enum ParseConfigT {
        PARSE_CONFIG_ID = 1,
        PARSE_BAUD_SPEED = 2,
        PARSE_TOTAL_ROW = 3,
        PARSE_SENSOR_DISPLAY = 4,
        PARSE_SENSOR_READ_WRITE = 5,
        PARSE_LOCAL_ALARM = 6,
        PARSE_CONTACT_PHONE = 7,
        PARSE_IS_WIFI = 8,
        PARSE_CONFIG_WIFI = 9,
        PARSE_CONFIG_ALIAS_TOTAL_ROW = 10,
        PARSE_CONFIG_ALIAS_DATA = 11,
        PARSE_IS_BLUETOOTH = 12,
        PARSE_AUTO_CLOSING = 13
    };

    enum ParseIntervalT {
        PARSE_MODBUS_BAUDRATE = 0,
        PARSE_MODBUS_INTERVAL = 1,
        PARSE_PUB_MODBUS_INTERVAL = 2
    };

public:
    ERaModbusEntry()
        : id(0)
        , prevId(0)
        , baudSpeed(DEFAULT_MODBUS_BAUD_SPEED)
        , isWiFi(false)
        , ssid {}
        , pass {}
        , hashID {}
        , isBluetooth(false)
        , autoClosing(false)
        , isUpdated(false)
        , sensorCount(0)
        , readConfigCount(0)
        , readConfigAliasCount(0)
        , writeOption {
            .enable = false,
            .len1 = 0,
            .len2 = 0
        }
        , modbusInterval {
            .delay = DEFAULT_MODBUS_INTERVAL,
            .prevMillis = 0
        }
        , pubInterval {
            .delay = DEFAULT_PUB_MODBUS_INTERVAL,
            .prevMillis = 0
        }
    {
        memset(this->hashID, 0, sizeof(this->hashID));
    }
    ~ERaModbusEntry()
    {}

    void parseConfig(const char* ptr);
    bool updateHashID(const char* hash, bool skip = false);
    bool equals(const char* hash);
    bool updated() {
        bool ret = this->isUpdated;
        this->isUpdated = false;
        return ret;
    }
    void deleteAll();
    void resize();

    bool operator == (const char* hash);
    bool operator != (const char* hash);

    unsigned int id;
    unsigned int prevId;
    unsigned int baudSpeed;
    bool isWiFi;
    char ssid[64];
    char pass[64];
    char hashID[37];
    bool isBluetooth;
    bool autoClosing;
    bool isUpdated;

    size_t sensorCount;
    ERaList<SensorDelay_t*> sensorDelay;
    size_t readConfigCount;
    ERaList<ModbusConfig_t*> modbusConfigParam;
    size_t readConfigAliasCount;
    ERaList<ModbusConfigAlias_t*> modbusConfigAliasParam;

    ModbusWriteOption_t writeOption;

    IntervalDelay_t modbusInterval;
    IntervalDelay_t pubInterval;

    static ERaModbusEntry*& config() {
        static ERaModbusEntry* _config = nullptr;
        return _config;
    }

    static ERaModbusEntry*& control() {
        static ERaModbusEntry* _control = nullptr;
        return _control;
    }

    static ERaModbusEntry* getConfig() {
        if (ERaModbusEntry::config() == nullptr) {
            ERaModbusEntry::config() = new ERaModbusEntry();
        }
        return ERaModbusEntry::config();
    }

    static ERaModbusEntry* getControl() {
        if (ERaModbusEntry::control() == nullptr) {
            ERaModbusEntry::control() = new ERaModbusEntry();
        }
        return ERaModbusEntry::control();
    }

private:
    void processParseConfig(int part, const char* ptr, size_t len);
    void processParseConfigId(const char* ptr, size_t len);
    void processParseConfigBaudSpeed(const char* ptr, size_t len);
    void processParseConfigTotalRow(const char* ptr, size_t len);
    void processParseConfigSensorDelay(const char* ptr, size_t len);
    void processParseConfigSensorReadWrite(const char* ptr, size_t len);
    void parseOneConfigSensorReadWrite(const char* ptr, size_t len, ModbusConfig_t& config);
    void actOneConfigSensorReadWrite(const int* ptr, size_t len, ModbusConfig_t& config);
    void processParseConfigLocalAlarm(const char* ptr, size_t len);
    void parseOneConfigLocalAlarm(const char* ptr, size_t len);
    void processParseStationContactPhone(const char* ptr, size_t len);
    void parseOneConfigPhoneNumber(const char* ptr, size_t len);
    void processParseIsWiFi(const char* ptr, size_t len);
    void processParseConfigWiFi(const char* ptr, size_t len);
    void processParseConfigAliasTotalRow(const char* ptr, size_t len);
    void processParseConfigAliasData(const char* ptr, size_t len);
    void processOneConfigAlias(const char* ptr, size_t len, ModbusConfigAlias_t& config);
    void parseOneConfigAlias(const char* ptr, size_t len, ModbusConfigAlias_t& config);
    void actOneConfigAlias(const int* ptr, size_t len, ModbusConfigAlias_t& config, const char* key);
    void processParseAction(const char* ptr, size_t len, ModbusConfigAlias_t& config);
    void parseOneAction(const char* ptr, size_t len, ModbusConfigAlias_t& config);
    void actOneAction(const int* ptr, size_t len, ModbusConfigAlias_t& config);
    void processParseIsEnableBluetooth(const char* ptr, size_t len);
    void processParseEnableAutoClosing(const char* ptr, size_t len);

    template <typename T>
    T* create() {
        T* ptr = (T*)ERA_MALLOC(sizeof(T));
        if (ptr != nullptr) {
            memset(ptr, 0, sizeof(T));
        }
        return ptr;
    }

    template <typename T>
    void clear(ERaList<T>& value) {
        const typename ERaList<T>::iterator* e = value.end();
        for (typename ERaList<T>::iterator* it = value.begin(); it != e; it = it->getNext()) {
            T& item = it->get();
            if (item == nullptr) {
                continue;
            }
            free(item);
            item = nullptr;
        }
        value.clear();
        value = {};
    }

    template <typename T>
    void resize(ERaList<T>& value, size_t capacity) {
        size_t size = value.size();
        typename ERaList<T>::iterator* it = nullptr;
        for (size_t i = size; i > capacity; --i) {
            it = value.get(i - 1);
            if (it == nullptr) {
                continue;
            }
            T& item = it->get();
            if (item != nullptr) {
                free(item);
                item = nullptr;
            }
            value.remove(it);
        }
    }

};

inline
void ERaModbusEntry::parseConfig(const char* ptr) {
    if (ptr == nullptr) {
        return;
    }
    if (!strlen(ptr)) {
        return;
    }

    this->isWiFi = false;
    this->sensorCount = 0;
    this->readConfigCount = 0;
    this->readConfigAliasCount = 0;

    size_t len = strlen(ptr);
    int part = 0;
    size_t position = 0;
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == ';') {
            part++;
            this->processParseConfig(part, ptr + position, i - position);
            position = i + 1;
        }
    }

    /*
    this->resize();
    */
}

inline
bool ERaModbusEntry::updateHashID(const char* hash, bool skip) {
    if (skip) {
        this->isUpdated = false;
    }
    if (hash == nullptr) {
        return this->isUpdated;
    }
    if (strcmp(this->hashID, hash)) {
        if (!skip) {
            this->isUpdated = true;
        }
        snprintf(this->hashID, sizeof(this->hashID), hash);
        return true;
    }
    return this->isUpdated;
}

inline
bool ERaModbusEntry::equals(const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return !strcmp(this->hashID, hash);
}

inline
void ERaModbusEntry::deleteAll() {
    this->clear(this->sensorDelay);
    this->clear(this->modbusConfigParam);
    this->clear(this->modbusConfigAliasParam);
    (*this) = {};
}

inline
void ERaModbusEntry::resize() {
    this->resize(this->sensorDelay, this->sensorCount);
    this->resize(this->modbusConfigParam, this->readConfigCount);
    this->resize(this->modbusConfigAliasParam, this->readConfigAliasCount);
}

inline
bool ERaModbusEntry::operator == (const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return !strcmp(this->hashID, hash);
}

inline
bool ERaModbusEntry::operator != (const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return strcmp(this->hashID, hash);
}

inline
void ERaModbusEntry::processParseConfig(int part, const char* ptr, size_t len) {
    switch(part) {
        case ParseConfigT::PARSE_CONFIG_ID:
            this->processParseConfigId(ptr, len);
            break;
        case ParseConfigT::PARSE_BAUD_SPEED:
            this->processParseConfigBaudSpeed(ptr, len);
            break;
        case ParseConfigT::PARSE_TOTAL_ROW:
            this->processParseConfigTotalRow(ptr, len);
            break;
        case ParseConfigT::PARSE_SENSOR_DISPLAY:
            this->processParseConfigSensorDelay(ptr, len);
            break;
        case ParseConfigT::PARSE_SENSOR_READ_WRITE:
            this->processParseConfigSensorReadWrite(ptr, len);
            break;
        case ParseConfigT::PARSE_LOCAL_ALARM:
            this->processParseConfigLocalAlarm(ptr, len);
            break;
        case ParseConfigT::PARSE_CONTACT_PHONE:
            this->processParseStationContactPhone(ptr, len);
            break;
        case ParseConfigT::PARSE_IS_WIFI:
            this->processParseIsWiFi(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_WIFI:
            this->processParseConfigWiFi(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_ALIAS_TOTAL_ROW:
            this->processParseConfigAliasTotalRow(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_ALIAS_DATA:
            this->processParseConfigAliasData(ptr, len);
            break;
        case ParseConfigT::PARSE_IS_BLUETOOTH:
            this->processParseIsEnableBluetooth(ptr, len);
            break;
        case ParseConfigT::PARSE_AUTO_CLOSING:
            this->processParseEnableAutoClosing(ptr, len);
            break;
        default:
            break;
    }
}

inline
void ERaModbusEntry::processParseConfigId(const char* ptr, size_t len) {
    LOC_BUFFER_PARSE

    this->prevId = this->id;
    memcpy(buf, ptr, len);
    this->id = atoi(buf);
    if (this->prevId != this->id) {
        this->isUpdated = true;
    }

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::processParseConfigBaudSpeed(const char* ptr, size_t len) {
    LOC_BUFFER_PARSE

    memcpy(buf, ptr, len);

    if (!strchr(buf, ',')) {
        this->baudSpeed = atoi(buf);
        FREE_BUFFER_PARSE
        return;
    }

    unsigned int step {0};
    char* token = strtok(buf, ",");
    while (token != nullptr) {
        switch (step++) {
            case ParseIntervalT::PARSE_MODBUS_BAUDRATE:
                this->baudSpeed = atoi(token);
                break;
            case ParseIntervalT::PARSE_MODBUS_INTERVAL:
                this->modbusInterval.delay = (atoi(token) < DEFAULT_MIN_MODBUS_INTERVAL ? DEFAULT_MIN_MODBUS_INTERVAL : atoi(token));
                break;
            case ParseIntervalT::PARSE_PUB_MODBUS_INTERVAL:
                this->pubInterval.delay = (atoi(token) < DEFAULT_MIN_MODBUS_INTERVAL ? DEFAULT_MIN_MODBUS_INTERVAL : atoi(token));
                break;
            default:
                break;
        }
        token = strtok(nullptr, ",");
    }

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::processParseConfigTotalRow(const char* ptr, size_t len) {
    ERA_FORCE_UNUSED(ptr);
    ERA_FORCE_UNUSED(len);
}

inline
void ERaModbusEntry::processParseConfigSensorDelay(const char* ptr, size_t len) {
    LOC_BUFFER_PARSE

    bool newItem {false};
    bool newSensor {true};
    size_t bufLen {0};

    /*
    this->clear(this->sensorDelay);
    */

    SensorDelay_t* sensor = nullptr;
    ERaList<SensorDelay_t *>::iterator* it = nullptr;
    for (size_t i = 0; i < len; ++i) { 
        buf[bufLen++] = ptr[i];

        if (ptr[i] == ',') {
            buf[--bufLen] = '\0';
            bufLen = 0;

            if (newSensor) {
                newItem = false;
                newSensor = false;
                sensor = nullptr;
                it = this->sensorDelay.get(this->sensorCount);
                if (it != nullptr) {
                    sensor = it->get();
                }
                if (sensor == nullptr) {
                    newItem = true;
                    sensor = this->create<SensorDelay_t>();
                }
                if (sensor != nullptr) {
                    sensor->address = atoi(buf);
                }
            }
            else {
                newSensor = true;
                if (sensor != nullptr) {
                    sensor->delay = atoi(buf);
                    if (newItem) {
                        this->sensorDelay.put(sensor);
                    }
                    this->sensorCount++;
                }
                if (this->sensorCount >= MAX_DEVICE_MODBUS) {
                    break;
                }
            }
        }
    }

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::processParseConfigSensorReadWrite(const char* ptr, size_t len) {
    bool newItem {false};
    size_t position {0};

    /*
    this->clear(this->modbusConfigParam);
    */

    ModbusConfig_t* config = nullptr;
    ERaList<ModbusConfig_t*>::iterator* it = nullptr;
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            newItem = false;
            config = nullptr;
            it = this->modbusConfigParam.get(this->readConfigCount);
            if (it != nullptr) {
                config = it->get();
            }
            if (config == nullptr) {
                newItem = true;
                config = this->create<ModbusConfig_t>();
            }
            if (config == nullptr) {
                continue;
            }
            this->parseOneConfigSensorReadWrite(ptr + position, i - position, *config);
            position = i + 1;
            if (newItem) {
                this->modbusConfigParam.put(config);
            }
            if (this->readConfigCount++ >= MAX_DEVICE_MODBUS) {
                break;
            }
        }
    }
}

inline
void ERaModbusEntry::parseOneConfigSensorReadWrite(const char* ptr, size_t len, ModbusConfig_t& config) {
    LOC_BUFFER_PARSE

    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};

    for (size_t i = 0; i < len; ++i) {
        buf[position++] = ptr[i];
        if (ptr[i] == ',') {
            buf[--position] = '\0';
            configParam[configIndex++] = atoi(buf);
            position = 0;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneConfigSensorReadWrite(configParam, configIndex, config);

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::actOneConfigSensorReadWrite(const int* ptr, size_t len, ModbusConfig_t& config) {
    if (ptr == nullptr) {
        return;
    }
    if (!len) {
        return;
    }

    config.addr = ptr[0];
    config.id = ptr[1];
    config.func = ptr[2];
    config.sa1 = ptr[3];
    config.sa2 = ptr[4];
    config.len1 = ptr[5];
    config.len2 = ptr[6];

    for (size_t i = 0; (i < len - 7) &&
        (i < sizeof(config.extra)); ++i) {
        config.extra[i] = ptr[i + 7];
    }
}

inline
void ERaModbusEntry::processParseConfigLocalAlarm(const char* ptr, size_t len) {
    size_t position = 0;

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            this->parseOneConfigLocalAlarm(ptr + position, i - position);
            position = i + 1;
        }
    }
}

inline
void ERaModbusEntry::parseOneConfigLocalAlarm(const char* ptr, size_t len) {
    ERA_FORCE_UNUSED(ptr);
    ERA_FORCE_UNUSED(len);
}

inline
void ERaModbusEntry::processParseStationContactPhone(const char* ptr, size_t len) {
    size_t position = 0;

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            this->parseOneConfigPhoneNumber(ptr + position, i - position);
            position = i + 1;
        }
    }
}

inline
void ERaModbusEntry::parseOneConfigPhoneNumber(const char* ptr, size_t len) {
    ERA_FORCE_UNUSED(ptr);
    ERA_FORCE_UNUSED(len);
}

inline
void ERaModbusEntry::processParseIsWiFi(const char* ptr, size_t len) {
    if (!len) {
        return;
    }
    this->isWiFi = ((ptr[0] == '1') ? true : false);
}

inline
void ERaModbusEntry::processParseConfigWiFi(const char* ptr, size_t len) {
    bool isSsid {true};

    memset(this->ssid, 0, sizeof(this->ssid));
    memset(this->pass, 0, sizeof(this->pass));

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == ',') {
            isSsid = false;
            continue;
        }
        if (isSsid) {
            snprintf(&this->ssid[strlen(this->ssid)],
                    (sizeof(this->ssid) - strlen(this->ssid)),
                    "%c", ptr[i]);
        }
        else {
            snprintf(&this->pass[strlen(this->pass)],
                    (sizeof(this->pass) - strlen(this->pass)),
                    "%c", ptr[i]);
        }
    }
}

inline
void ERaModbusEntry::processParseConfigAliasTotalRow(const char* ptr, size_t len) {
    ERA_FORCE_UNUSED(ptr);
    ERA_FORCE_UNUSED(len);
}

inline
void ERaModbusEntry::processParseConfigAliasData(const char* ptr, size_t len) {
    bool newItem {false};
    size_t position {0};

    /*
    this->clear(this->modbusConfigAliasParam);
    */

    ModbusConfigAlias_t* config = nullptr;
    ERaList<ModbusConfigAlias_t*>::iterator* it = nullptr;
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            newItem = false;
            config = nullptr;
            it = this->modbusConfigAliasParam.get(this->readConfigAliasCount);
            if (it != nullptr) {
                config = it->get();
            }
            if (config == nullptr) {
                newItem = true;
                config = this->create<ModbusConfigAlias_t>();
            }
            if (config == nullptr) {
                continue;
            }
            this->processOneConfigAlias(ptr + position, i - position, *config);
            position = i + 1;
            if (newItem) {
                this->modbusConfigAliasParam.put(config);
            }
            if (this->readConfigAliasCount++ >= MAX_DEVICE_MODBUS) {
                break;
            }
        }
    }
}

inline
void ERaModbusEntry::processOneConfigAlias(const char* ptr, size_t len, ModbusConfigAlias_t& config) {
    if (len < 2) {
        return;
    }

    size_t position {0};

    for (size_t i = 0; i < len - 1; ++i) {
        if (ptr[i] == ',' && ptr[i + 1] == '-') {
            position = i + 1;
            this->parseOneConfigAlias(ptr, i + 1, config);
            this->processParseAction(ptr + position, len - position, config);
            break;
        }
    }
}

inline
void ERaModbusEntry::parseOneConfigAlias(const char* ptr, size_t len, ModbusConfigAlias_t& config) {
    if (len < 36) {
        return;
    }

    LOC_BUFFER_PARSE

    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};
    bool isAlias {false};
    char key[37] {0};

    for (size_t i = 0; i < len; ++i) {
        buf[position++] = ptr[i];
        if (ptr[i] == ',') {
            buf[--position] = '\0';
            if (isAlias) {
                memcpy(key, buf, 36);
            }
            else {
                configParam[configIndex++] = atoi(buf);
            }
            position = 0;
            isAlias = true;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneConfigAlias(configParam, configIndex, config, key);

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::actOneConfigAlias(const int* ptr, size_t len, ModbusConfigAlias_t& config, const char* key) {
    if (!len) {
        return;
    }
    if (strlen(key) != 36) {
        return;
    }
    config.id = ptr[0];
    memcpy(config.key, key, 36);
}

inline
void ERaModbusEntry::processParseAction(const char* ptr, size_t len, ModbusConfigAlias_t& config) {
    size_t position {0};
    size_t numHyphen {0};

    config.readActionCount = 0;

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '-') {
            ++numHyphen;

            if (numHyphen == 2) {
                numHyphen = 0;
                this->parseOneAction(ptr + position + 1, i - position, config);
                position = i + 1;
                config.readActionCount++;
            }
        }
    }
}

inline
void ERaModbusEntry::parseOneAction(const char* ptr, size_t len, ModbusConfigAlias_t& config) {
    LOC_BUFFER_PARSE

    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};

    for (size_t i = 0; i < len; ++i) {
        buf[position++] = ptr[i];
        if (ptr[i] == ',') {
            buf[--position] = '\0';
            configParam[configIndex++] = atoi(buf);
            position = 0;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneAction(configParam, configIndex, config);

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::actOneAction(const int* ptr, size_t len, ModbusConfigAlias_t& config) {
    Action_t& action = config.action[config.readActionCount];

    action.id = ptr[0];
    action.len1 = ptr[1];
    action.len2 = ptr[2];
    for (size_t i = 0; i < len - 3; ++i) {
        action.extra[i] = ptr[i + 3];
    }
}

inline
void ERaModbusEntry::processParseIsEnableBluetooth(const char* ptr, size_t len) {
    if (!len) {
        return;
    }
    this->isBluetooth = ((ptr[0] == '1') ? true : false);
}

inline
void ERaModbusEntry::processParseEnableAutoClosing(const char* ptr, size_t len) {
    if (!len) {
        return;
    }
    this->autoClosing = ((ptr[0] == '1') ? true : false);
}

class ERaScanEntry
{
    enum ParseConfigT {
        PARSE_CONFIG_RANGE = 1,
        PARSE_CONFIG_NUMBER_SCAN = 2
    };

public:
    ERaScanEntry()
    {}
    ~ERaScanEntry()
    {}

    uint8_t start;
    uint8_t end;
    uint8_t numberScan;
    uint8_t numberDevice;
    uint8_t addr[MAX_DEVICE_MODBUS];

    void parseConfig(const char* ptr);

    static ERaScanEntry*& instance() {
        static ERaScanEntry* _instance = nullptr;
        return _instance;
    }

    static ERaScanEntry* getInstance() {
        if (ERaScanEntry::instance() == nullptr) {
            ERaScanEntry::instance() = new ERaScanEntry();
        }
        return ERaScanEntry::instance();
    }

protected:
private:
    void processParseConfig(int part, const char* ptr, size_t len);
    void processParseConfigRange(const char* ptr, size_t len);
    void processParseConfigNumberScan(const char* ptr, size_t len);
};

inline
void ERaScanEntry::parseConfig(const char* ptr) {
    if (ptr == nullptr) {
        return;
    }
    if (!strlen(ptr)) {
        return;
    }

    this->numberDevice = 0;

    size_t len = strlen(ptr);
    int part = 0;
    size_t position = 0;
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == ';') {
            part++;
            this->processParseConfig(part, ptr + position, i - position);
            position = i + 1;
        }
    }
}

inline
void ERaScanEntry::processParseConfig(int part, const char* ptr, size_t len) {
    switch(part) {
        case ParseConfigT::PARSE_CONFIG_RANGE:
            this->processParseConfigRange(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_NUMBER_SCAN:
            this->processParseConfigNumberScan(ptr, len);
            break;
        default:
            break;
    }
}

inline
void ERaScanEntry::processParseConfigRange(const char* ptr, size_t len) {
    size_t pos {0};
    bool getEnd {false};
    char startRange[10] {0};
    char endRange[10] {0};

    for (size_t i = 0; (i < len) && (pos < 10); ++i) {
        if (ptr[i] == ',') {
            startRange[pos] = 0;
            pos = 0;
            getEnd = true;
            continue;
        }
        if (!getEnd) {
            startRange[pos++] = ptr[i];
        }
        else {
            endRange[pos++] = ptr[i];
        }
    }
    endRange[pos] = 0;

    this->start = atoi(startRange);
    this->end = atoi(endRange);
}

inline
void ERaScanEntry::processParseConfigNumberScan(const char* ptr, size_t len) {
    size_t pos {0};
    char buf[10] {0};

    for (size_t i = 0; (i < len) && (pos < 10); ++i) {
        buf[pos++] = ptr[i];
    }
    buf[pos] = 0;

    this->numberScan = atoi(buf);
}

#endif /* INC_ERA_PARSE_HPP_ */
