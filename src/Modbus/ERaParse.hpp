#ifndef INC_ERA_PARSE_HPP_
#define INC_ERA_PARSE_HPP_

#include <string.h>
#include <ERa/ERaDebug.hpp>
#include <ERa/ERaHelpersDef.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERacJSON.hpp>
#include <Modbus/ERaParseKey.hpp>

#ifndef MAX_DEVICE_MODBUS
    #define MAX_DEVICE_MODBUS           20
#endif

#ifndef MAX_CONFIG_MODBUS
    #define MAX_CONFIG_MODBUS           50
#endif

#ifndef MAX_ALIAS_MODBUS
    #define MAX_ALIAS_MODBUS            100
#endif

#ifndef MAX_ACTION_MODBUS
    #define MAX_ACTION_MODBUS           10
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

#define MODBUS_VERSION_SUPPORT_JSON     9

typedef struct __SensorDelay_t {
    ERaUInt_t delay;
    ERaUInt_t address;
} SensorDelay_t;

typedef struct __IPSlave_t {
    union {
        uint8_t bytes[4];
        uint32_t dword;
    } ip;
    uint16_t port;
} IPSlave_t;

typedef struct __ModbusConfig_t {
    ERaUInt_t id;
    uint8_t addr;
    uint8_t func;
    uint8_t sa1;
    uint8_t sa2;
    uint8_t len1;
    uint8_t len2;
    uint8_t extra[10];
    uint16_t delay;
    IPSlave_t ipSlave;
    uint8_t totalFail;
    float scaleWrite;
} ModbusConfig_t;

typedef struct __Action_t {
    ERaUInt_t id;
    uint8_t len1;
    uint8_t len2;
    uint8_t extra[10];
    uint16_t delay;
} Action_t;

typedef struct __ModbusConfigAlias_t {
    ERaUInt_t readActionCount;
    ERaUInt_t id;
    char key[37];
    Action_t action[MAX_ACTION_MODBUS];
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

#include <Modbus/ERaDefineModbus.hpp>

class ERaScanEntry;

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
        PARSE_TCP_IP_CONFIG = 13,
        PARSE_AUTO_CLOSING = 14
    };

    enum ParseIntervalT {
        PARSE_MODBUS_BAUDRATE = 0,
        PARSE_MODBUS_INTERVAL = 1,
        PARSE_PUB_MODBUS_INTERVAL = 2
    };

    enum ConnectionTypeT {
        CONNECTION_TYPE_RTU = 0,
        CONNECTION_TYPE_TCPIP = 1
    };

    friend class ERaScanEntry;

public:
    ERaModbusEntry()
        : id(0UL)
        , prevId(0UL)
        , baudSpeed(DEFAULT_MODBUS_BAUD_SPEED)
        , isJson(false)
        , isWiFi(false)
        , ssid {}
        , pass {}
        , hashID {}
        , isBluetooth(false)
        , autoClosing(false)
        , isUpdated(false)
        , hasTcpIp(false)
        , sensorCount(0)
        , readConfigCount(0)
        , readConfigAliasCount(0)
        , writeOption {
            .enable = false,
            .len1 = 0,
            .len2 = 0,
            .extra = {}
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

    void parseConfig(const void* ptr, bool json = false);
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

    ERaUInt_t id;
    ERaUInt_t prevId;
    ERaUInt_t baudSpeed;
    bool isJson;
    bool isWiFi;
    char ssid[64];
    char pass[64];
    char hashID[37];
    bool isBluetooth;
    bool autoClosing;
    bool isUpdated;
    bool hasTcpIp;

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
    void parseOneTcpIp(const char* ptr, size_t len);
    void processParseTcpIpConfig(const char* ptr, size_t len);
    void processParseEnableAutoClosing(const char* ptr, size_t len);

    void processParseConfigJson(const void* ptr);
    void processParseConfigSensorDelayJson(const cJSON* const root, uint8_t addr);
    void processParseConfigSensorIpJson(const cJSON* const root, uint8_t type, IPSlave_t& ipSlave);

    ModbusConfig_t* processParseConfigSensorParamJson(const cJSON* const root, uint8_t addr, IPSlave_t& ipSlave, ModbusConfig_t* prevConfig);
    void processParseConfigSensorParamsJson(const cJSON* const root, uint8_t addr, IPSlave_t& ipSlave);

    void processParseConfigSensorActionItemJson(const cJSON* const root, ModbusConfigAlias_t& config);
    void processParseConfigSensorActionItemsJson(const cJSON* const root, ModbusConfigAlias_t& config);
    void processParseConfigSensorActionJson(const cJSON* const root);
    void processParseConfigSensorActionsJson(const cJSON* const root);

    void processParseConfigSensorJson(const cJSON* const root);
    void processParseConfigSensorsJson(const cJSON* const root);

    static inline
    bool portFromString(IPSlave_t& ip, const char* port) {
        uint32_t acc = 0;
        while (*port) {
            char c = *port++;
            if ((c >= '0') && (c <= '9')) {
                acc = (acc * 10) + (c - '0');
                if (acc > 65535) {
                    return false;
                }
            }
            else if (c == '"') {
                continue;
            }
            else {
                break;
            }
        }
        ip.port = acc;
        return true;
    }

    static inline
    bool ipFromString(IPSlave_t& ip, const char* address) {
        uint16_t acc = 0;
        uint8_t dots = 0;

        while (*address) {
            char c = *address++;
            if ((c >= '0') && (c <= '9')) {
                acc = (acc * 10) + (c - '0');
                if (acc > 255) {
                    return false;
                }
            }
            else if (c == '.') {
                if (dots == 3) {
                    return false;
                }
                ip.ip.bytes[dots++] = acc;
                acc = 0;
            }
            else if (c == '"') {
                continue;
            }
            else if (c == ':') {
                ERaModbusEntry::portFromString(ip, address);
                break;
            }
            else {
                return false;
            }
        }

        if (dots != 3) {
            return false;
        }
        if (ip.port == 0) {
            ip.port = 502;
        }
        ip.ip.bytes[3] = acc;
        return true;
    }

    template <size_t len>
    static inline
    void stringToUint8(uint8_t(&arr)[len], const char* str) {
        if (str == nullptr) {
            return;
        }
        char hex[3] {0};
        size_t size = (strlen(str) / 2);
        for (size_t i = 0; (i < len) && (i < size); ++i) {
            hex[0] = str[i * 2];
            hex[1] = str[(i * 2) + 1];
            arr[i] = strtol(hex, nullptr, 16);
        }
    }

    template <typename T>
    T* create() {
        T* ptr = (T*)ERA_MALLOC(sizeof(T));
        if (ptr != nullptr) {
            memset((void*)ptr, 0, sizeof(T));
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

    template <typename T>
    T find(ERaList<T>& value, ERaUInt_t id) {
        const typename ERaList<T>::iterator* e = value.end();
        for (typename ERaList<T>::iterator* it = value.begin(); it != e; it = it->getNext()) {
            T& item = it->get();
            if (item == nullptr) {
                continue;
            }
            if (item->id == id) {
                return item;
            }
        }
        return nullptr;
    }

};

inline
void ERaModbusEntry::parseConfig(const void* ptr, bool json) {
    if (ptr == nullptr) {
        return;
    }

    this->isJson = json;
    this->isWiFi = false;
    this->hasTcpIp = false;
    this->sensorCount = 0;
    this->readConfigCount = 0;
    this->readConfigAliasCount = 0;

    if (json) {
        return this->processParseConfigJson(ptr);
    }

    const char* config = (const char*)ptr;
    if (!strlen(config)) {
        return;
    }

    size_t len = strlen(config);
    int part = 0;
    size_t position = 0;
    for (size_t i = 0; i < len; ++i) {
        if (config[i] == ';') {
            part++;
            this->processParseConfig(part, config + position, i - position);
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
        case ParseConfigT::PARSE_TCP_IP_CONFIG:
            this->processParseTcpIpConfig(ptr, len);
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
                this->modbusInterval.delay = ((atoi(token) < DEFAULT_MIN_MODBUS_INTERVAL) ?
                                               DEFAULT_MIN_MODBUS_INTERVAL : atoi(token));
                break;
            case ParseIntervalT::PARSE_PUB_MODBUS_INTERVAL:
                this->pubInterval.delay = ((atoi(token) < DEFAULT_MIN_MODBUS_INTERVAL) ?
                                            DEFAULT_MIN_MODBUS_INTERVAL : atoi(token));
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
    ERaList<SensorDelay_t*>::iterator* it = nullptr;
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
            if (this->readConfigCount++ >= MAX_CONFIG_MODBUS) {
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

    config.scaleWrite = 1.0f;
    config.ipSlave.ip.dword = 0UL;
    config.ipSlave.port = 0;
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
            if (this->readConfigAliasCount++ >= MAX_ALIAS_MODBUS) {
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
            if (position > 36) {
                break;
            }
            if (isAlias) {
                memcpy(key, buf, strlen(buf) + 1);
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
    if (strlen(key) > 36) {
        return;
    }
    config.id = ptr[0];
    memcpy(config.key, key, strlen(key) + 1);
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
void ERaModbusEntry::parseOneTcpIp(const char* ptr, size_t len) {
    if (!len) {
        return;
    }

    LOC_BUFFER_PARSE

    size_t bufLen {0};
    ModbusConfig_t* config = nullptr;
    for (size_t i = 0; i < len; ++i) {
        buf[bufLen++] = ptr[i];
        if (ptr[i] == ',') {
            buf[--bufLen] = '\0';
            bufLen = 0;
            config = this->find(this->modbusConfigParam, atoi(buf));
            if (config == nullptr) {
                break;
            }
            if (!ERaModbusEntry::ipFromString(config->ipSlave, ptr + i + 1)) {
                config->ipSlave.port = 0;
                config->ipSlave.ip.dword = 0UL;
            }
            else {
                this->hasTcpIp = true;
            }
            break;
        }
    }

    FREE_BUFFER_PARSE
}

inline
void ERaModbusEntry::processParseTcpIpConfig(const char* ptr, size_t len) {
    size_t position {0};

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == '-') {
            this->parseOneTcpIp(ptr + position, i - position);
            position = i + 1;
        }
    }
}

inline
void ERaModbusEntry::processParseEnableAutoClosing(const char* ptr, size_t len) {
    if (!len) {
        return;
    }
    this->autoClosing = ((ptr[0] == '1') ? true : false);
}

inline
void ERaModbusEntry::processParseConfigJson(const void* ptr) {
    const cJSON* root = (const cJSON*)ptr;
    if (!cJSON_IsObject(root)) {
        return;
    }

    cJSON* version = cJSON_GetObjectItem(root, MODBUS_VERSION_KEY);
    if (!cJSON_IsNumber(version)) {
        return;
    }
    if (version->valueint < MODBUS_VERSION_SUPPORT_JSON) {
        return;
    }

    cJSON* baud = cJSON_GetObjectItem(root, MODBUS_BAUD_SPEED_KEY);
    if (cJSON_IsNumber(baud)) {
        this->baudSpeed = baud->valueint;
    }

    cJSON* sensors = cJSON_GetObjectItem(root, MODBUS_SENSORS_KEY);
    if (cJSON_IsArray(sensors)) {
        this->processParseConfigSensorsJson(sensors);
    }
}

inline
void ERaModbusEntry::processParseConfigSensorDelayJson(const cJSON* const root, uint8_t addr) {
    if (this->sensorCount >= MAX_DEVICE_MODBUS) {
        return;
    }

    bool newItem {false};
    SensorDelay_t* sensor = nullptr;
    ERaList<SensorDelay_t*>::iterator* it = nullptr;

    it = this->sensorDelay.get(this->sensorCount);
    if (it != nullptr) {
        sensor = it->get();
    }
    if (sensor == nullptr) {
        newItem = true;
        sensor = this->create<SensorDelay_t>();
    }
    if (sensor == nullptr) {
        return;
    }

    cJSON* delay = cJSON_GetObjectItem(root, MODBUS_SENSOR_DELAY_KEY);
    if (cJSON_IsNumber(delay)) {
        sensor->delay = delay->valueint;
    }
    else {
        sensor->delay = 20UL;
    }
    sensor->address = addr;

    if (newItem) {
        this->sensorDelay.put(sensor);
    }
    this->sensorCount++;
}

inline
void ERaModbusEntry::processParseConfigSensorIpJson(const cJSON* const root, uint8_t type, IPSlave_t& ipSlave) {
    memset(&ipSlave, 0, sizeof(ipSlave));

    switch (type) {
        case ConnectionTypeT::CONNECTION_TYPE_RTU:
            break;
        case ConnectionTypeT::CONNECTION_TYPE_TCPIP: {
            cJSON* port = cJSON_GetObjectItem(root, MODBUS_SENSOR_PORT_KEY);
            if (cJSON_IsNumber(port)) {
                ipSlave.port = port->valueint;
            }
            cJSON* ip = cJSON_GetObjectItem(root, MODBUS_SENSOR_IP_KEY);
            if (!cJSON_IsString(ip)) {
                ipSlave.port = 0;
                break;
            }
            if (!ERaModbusEntry::ipFromString(ipSlave, ip->valuestring)) {
                ipSlave.port = 0;
                ipSlave.ip.dword = 0UL;
            }
            else {
                this->hasTcpIp = true;
            }
        }
            break;
    }
}

inline
ModbusConfig_t* ERaModbusEntry::processParseConfigSensorParamJson(const cJSON* const root, uint8_t addr, IPSlave_t& ipSlave, ModbusConfig_t* prevConfig) {
    if (this->readConfigCount >= MAX_CONFIG_MODBUS) {
        return prevConfig;
    }

    bool newItem {false};
    ModbusConfig_t* config = nullptr;
    ERaList<ModbusConfig_t*>::iterator* it = nullptr;

    it = this->modbusConfigParam.get(this->readConfigCount);
    if (it != nullptr) {
        config = it->get();
    }
    if (config == nullptr) {
        newItem = true;
        config = this->create<ModbusConfig_t>();
    }
    if (config == nullptr) {
        return prevConfig;
    }

    ModbusConfig_t newConfig {};

    newConfig.addr = addr;

    cJSON* id = cJSON_GetObjectItem(root, MODBUS_PARAM_ID_KEY);
    if (cJSON_IsNumber(id)) {
        newConfig.id = id->valueint;
    }

    cJSON* func = cJSON_GetObjectItem(root, MODBUS_PARAM_FUNCTION_KEY);
    if (cJSON_IsNumber(func)) {
        newConfig.func = func->valueint;
    }

    cJSON* reg = cJSON_GetObjectItem(root, MODBUS_PARAM_REGISTER_KEY);
    if (cJSON_IsNumber(reg)) {
        newConfig.sa1 = HI_WORD(reg->valueint);
        newConfig.sa2 = LO_WORD(reg->valueint);
    }

    cJSON* len = cJSON_GetObjectItem(root, MODBUS_PARAM_LENGTH_KEY);
    if (cJSON_IsNumber(len)) {
        newConfig.len1 = HI_WORD(len->valueint);
        newConfig.len2 = LO_WORD(len->valueint);
    }

    cJSON* extra = cJSON_GetObjectItem(root, MODBUS_PARAM_EXTRA_KEY);
    if (cJSON_IsString(extra)) {
        ERaModbusEntry::stringToUint8(newConfig.extra, extra->valuestring);
    }

    cJSON* scaleWrite = cJSON_GetObjectItem(root, MODBUS_PARAM_SCALE_WRITE_KEY);
    if (cJSON_IsNumber(scaleWrite)) {
        newConfig.scaleWrite = scaleWrite->valuedouble;
    }
    else {
        newConfig.scaleWrite = 1.0f;
    }

    newConfig.ipSlave.port = ipSlave.port;
    newConfig.ipSlave.ip.dword = ipSlave.ip.dword;

    if (prevConfig == nullptr) {
        memcpy(config, &newConfig, sizeof(ModbusConfig_t));
    }
    else if ((prevConfig->addr == newConfig.addr) &&
             (prevConfig->func == newConfig.func) &&
             ((BUILD_WORD(prevConfig->sa1, prevConfig->sa2) +
               BUILD_WORD(prevConfig->len1, prevConfig->len2)) ==
               BUILD_WORD(newConfig.sa1, newConfig.sa2))) {
        prevConfig->len1 += newConfig.len1;
        prevConfig->len2 += newConfig.len2;
        if (newItem) {
            free(config);
            config = nullptr;
        }
        return prevConfig;
    }
    else {
        memcpy(config, &newConfig, sizeof(ModbusConfig_t));
    }

    if (newItem) {
        this->modbusConfigParam.put(config);
    }
    this->readConfigCount++;
    return config;
}

inline
void ERaModbusEntry::processParseConfigSensorParamsJson(const cJSON* const root, uint8_t addr, IPSlave_t& ipSlave) {
    size_t size = cJSON_GetArraySize(root);

    ModbusConfig_t* config = nullptr;
    for (size_t i = 0; i < size; ++i) {
        cJSON* param = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(param)) {
            continue;
        }
        config = this->processParseConfigSensorParamJson(param, addr, ipSlave, config);
    }
}

inline
void ERaModbusEntry::processParseConfigSensorActionItemJson(const cJSON* const root, ModbusConfigAlias_t& config) {
    if (config.readActionCount >= MAX_ACTION_MODBUS) {
        return;
    }
    Action_t& action = config.action[config.readActionCount];

    cJSON* id = cJSON_GetObjectItem(root, MODBUS_ACTION_ITEM_CONFIG_ID_KEY);
    if (cJSON_IsNumber(id)) {
        action.id = id->valueint;
    }

    cJSON* len = cJSON_GetObjectItem(root, MODBUS_ACTION_ITEM_LENGTH_KEY);
    if (cJSON_IsNumber(len)) {
        action.len1 = HI_WORD(len->valueint);
        action.len2 = LO_WORD(len->valueint);
    }

    cJSON* extra = cJSON_GetObjectItem(root, MODBUS_ACTION_ITEM_EXTRA_KEY);
    if (cJSON_IsString(extra)) {
        ERaModbusEntry::stringToUint8(action.extra, extra->valuestring);
    }

    cJSON* delay = cJSON_GetObjectItem(root, MODBUS_ACTION_ITEM_DELAY_KEY);
    if (cJSON_IsNumber(delay)) {
        action.delay = delay->valueint;
    }

    config.readActionCount++;
}

inline
void ERaModbusEntry::processParseConfigSensorActionItemsJson(const cJSON* const root, ModbusConfigAlias_t& config) {
    size_t size = cJSON_GetArraySize(root);

    config.readActionCount = 0;
    for (size_t i = 0; i < size; ++i) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(item)) {
            continue;
        }
        this->processParseConfigSensorActionItemJson(item, config);
    }
}

inline
void ERaModbusEntry::processParseConfigSensorActionJson(const cJSON* const root) {
    if (this->readConfigAliasCount >= MAX_ALIAS_MODBUS) {
        return;
    }

    bool newItem {false};
    ModbusConfigAlias_t* config = nullptr;
    ERaList<ModbusConfigAlias_t*>::iterator* it = nullptr;

    it = this->modbusConfigAliasParam.get(this->readConfigAliasCount);
    if (it != nullptr) {
        config = it->get();
    }
    if (config == nullptr) {
        newItem = true;
        config = this->create<ModbusConfigAlias_t>();
    }
    if (config == nullptr) {
        return;
    }

    cJSON* id = cJSON_GetObjectItem(root, MODBUS_ACTION_ID_KEY);
    if (cJSON_IsNumber(id)) {
        config->id = id->valueint;
    }

    cJSON* key = cJSON_GetObjectItem(root, MODBUS_ACTION_KEY_KEY);
    if (cJSON_IsString(key) && (strlen(key->valuestring) <= 36)) {
        memcpy(config->key, key->valuestring, strlen(key->valuestring) + 1);
    }

    cJSON* items = cJSON_GetObjectItem(root, MODBUS_ACTION_ITEMS_KEY);
    if (cJSON_IsArray(items)) {
        this->processParseConfigSensorActionItemsJson(items, *config);
    }

    if (newItem) {
        this->modbusConfigAliasParam.put(config);
    }
    this->readConfigAliasCount++;
}

inline
void ERaModbusEntry::processParseConfigSensorActionsJson(const cJSON* const root) {
    size_t size = cJSON_GetArraySize(root);

    for (size_t i = 0; i < size; ++i) {
        cJSON* action = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(action)) {
            continue;
        }
        this->processParseConfigSensorActionJson(action);
    }
}

inline
void ERaModbusEntry::processParseConfigSensorJson(const cJSON* const root) {
    cJSON* type = cJSON_GetObjectItem(root, MODBUS_SENSOR_TYPE_KEY);
    if (!cJSON_IsNumber(type)) {
        return;
    }

    cJSON* address = cJSON_GetObjectItem(root, MODBUS_SENSOR_ADDRESS_KEY);
    if (!cJSON_IsNumber(address)) {
        return;
    }

    IPSlave_t ipSlave {};
    this->processParseConfigSensorDelayJson(root, address->valueint);
    this->processParseConfigSensorIpJson(root, type->valueint, ipSlave);

    cJSON* configs = cJSON_GetObjectItem(root, MODBUS_SENSOR_CONFIGS_KEY);
    if (!cJSON_IsArray(configs)) {
        return;
    }
    this->processParseConfigSensorParamsJson(configs, address->valueint, ipSlave);

    cJSON* actions = cJSON_GetObjectItem(root, MODBUS_SENSOR_ALIAS_CONFIGS_KEY);
    if (!cJSON_IsArray(actions)) {
        return;
    }
    this->processParseConfigSensorActionsJson(actions);
}

inline
void ERaModbusEntry::processParseConfigSensorsJson(const cJSON* const root) {
    size_t size = cJSON_GetArraySize(root);

    for (size_t i = 0; i < size; ++i) {
        cJSON* sensor = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(sensor)) {
            continue;
        }
        this->processParseConfigSensorJson(sensor);
    }
}

class ERaScanEntry
{
    enum ParseConfigT {
        PARSE_CONFIG_RANGE = 1,
        PARSE_CONFIG_NUMBER_SCAN = 2,
        PARSE_TCP_IP_CONFIG = 3
    };

public:
    ERaScanEntry()
        : start(0)
        , end(0)
        , numberScan(0)
        , numberDevice(0)
        , addr {}
        , ipSlave {}
    {}
    ~ERaScanEntry()
    {}

    uint8_t start;
    uint8_t end;
    uint8_t numberScan;
    uint8_t numberDevice;
    uint8_t addr[MAX_DEVICE_MODBUS];
    IPSlave_t ipSlave;

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
    void processParseTcpIpConfig(const char* ptr, size_t len);
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
        case ParseConfigT::PARSE_TCP_IP_CONFIG:
            this->processParseTcpIpConfig(ptr, len);
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

inline
void ERaScanEntry::processParseTcpIpConfig(const char* ptr, size_t len) {
    if (!len) {
        return;
    }

    if (!ERaModbusEntry::ipFromString(this->ipSlave, ptr)) {
        this->ipSlave.port = 0;
        this->ipSlave.ip.dword = 0UL;
    }
}

#endif /* INC_ERA_PARSE_HPP_ */
