#ifndef INC_ERA_PARSE_HPP_
#define INC_ERA_PARSE_HPP_

#include <Utility/ERaQueue.hpp>

#define MAXIMUM_MODBUS_DEVICES  20
#define BAUD_SPEED_DEFAULT      9600
#define REQUEST_DELAY_DEFAULT   1000
#define MODBUS_DELAY_DEFAULT 	10000
#define PUSH_DELAY_DEFAULT 	    60000
#define MIN_DELAY_DEFAULT		1000

typedef struct __SensorDelay_t {
	int delay;
	int address;
} SensorDelay_t;

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
	char ipSlave[20];
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

typedef struct __TimerDelay_t {
	uint32_t delay;
	unsigned long prevMillis;
} TimerDelay_t;

class ERaApplication {
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
        PARSE_IS_BLUETOOTH = 12
    };

    enum ParseDelayT {
        PARSE_BAUDRATE = 0,
        PARSE_REQUEST_DELAY = 1,
        PARSE_MODBUS_DELAY = 2,
        PARSE_PUSH_DELAY = 3
    };

public:
    ERaApplication();
    ~ERaApplication()
    {}

    void parseConfig(char* ptr);
    void deleteAll();

    unsigned int id;
    unsigned int baudSpeed;
    bool isWifi;
    char ssid[64];
    char password[64];
    bool isBluetooth;

    size_t sensorCount;
    ERaList<SensorDelay_t> sensorDelay;
    size_t readConfigCount;
    ERaList<ModbusConfig_t> modbusConfigParam;
    size_t readConfigAliasCount;
    ERaList<ModbusConfigAlias_t> modbusConfigAliasParam;

    TimerDelay_t requestDelay;
    TimerDelay_t modbusDelay;
    TimerDelay_t pushDelay;

    static ERaApplication config;
    static ERaApplication control;

private:
    void processParseConfig(int part, char* ptr, size_t len);
    void processParseConfigId(char* ptr, size_t len);
    void processParseConfigBaudSpeed(char* ptr, size_t len);
    void processParseConfigTotalRow(char* ptr, size_t len);
    void processParseConfigSensorDelay(char* ptr, size_t len);
    void processParseConfigSensorReadWrite(char* ptr, size_t len);
    void parseOneConfigSensorReadWrite(char* ptr, size_t len, ModbusConfig_t& config);
    void actOneConfigSensorReadWrite(int* ptr, size_t len, ModbusConfig_t& config);
    void processParseConfigLocalAlarm(char* ptr, size_t len);
    void parseOneConfigLocalAlarm(char* ptr, size_t len);
    void processParseStationContactPhone(char* ptr, size_t len);
    void parseOneConfigPhoneNumber(char* ptr, size_t len);
    void processParseIsWifi(char* ptr, size_t len);
    void processParseConfigWifi(char* ptr, size_t len);
    void processParseConfigAliasTotalRow(char* ptr, size_t len);
    void processParseConfigAliasData(char* ptr, size_t len);
    void processOneConfigAlias(char* ptr, size_t len, ModbusConfigAlias_t& config);
    void parseOneConfigAlias(char* ptr, size_t len, ModbusConfigAlias_t& config);
    void actOneConfigAlias(int* ptr, size_t len, ModbusConfigAlias_t& config, const char* key);
    void processParseAction(char* ptr, size_t len, ModbusConfigAlias_t& config);
    void parseOneAction(char* ptr, size_t len, ModbusConfigAlias_t& config);
    void actOneAction(int* ptr, size_t len, ModbusConfigAlias_t& config);
    void processParseIsEnableBluetooth(char* ptr, size_t len);
};

#endif /* INC_ERA_PARSE_HPP_ */
