#include <string.h>
#include <Modbus/MVPParse.hpp>

MVPApplication MVPApplication::config {};
MVPApplication MVPApplication::control {};

MVPApplication::MVPApplication()
    : baudSpeed(BAUD_SPEED_DEFAULT)
    , requestDelay {
        .delay = REQUEST_DELAY_DEFAULT
    }
    , modbusDelay {
        .delay = MODBUS_DELAY_DEFAULT
    }
    , pushDelay {
        .delay = PUSH_DELAY_DEFAULT
    }
{

}

void MVPApplication::parseConfig(char* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    if (!strlen(ptr)) {
        return;
    }

    this->sensorCount = 0;
    this->readConfigCount = 0;
    this->readConfigAliasCount = 0;

    size_t len = strlen(ptr);
    int part = 0;
    size_t position = 0;
    for (int i = 0; i < len; ++i) {
        if (ptr[i] == ';') {
            part++;
            this->processParseConfig(part, ptr + position, i - position);
            position = i + 1;
        }
    }
}

void MVPApplication::deleteAll() {
    this->sensorDelay.clear();
    this->modbusConfigParam.clear();
    this->modbusConfigAliasParam.clear();
    *this = {};
}

void MVPApplication::processParseConfig(int part, char* ptr, size_t len)
{
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
			this->processParseIsWifi(ptr, len);
			break;
		case ParseConfigT::PARSE_CONFIG_WIFI:
    		this->processParseConfigWifi(ptr, len);
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
		default:
			break;
	}
}

void MVPApplication::processParseConfigId(char* ptr, size_t len)
{
    char buff[len + 1] {0};
    memcpy(buff, ptr, len);
    this->id = atoi(buff);
}

void MVPApplication::processParseConfigBaudSpeed(char* ptr, size_t len)
{
    char buff[len + 1] {0};
    memcpy(buff, ptr, len);

    this->baudSpeed = BAUD_SPEED_DEFAULT;
    this->requestDelay.delay = REQUEST_DELAY_DEFAULT;
    this->modbusDelay.delay = MODBUS_DELAY_DEFAULT;
    this->pushDelay.delay = PUSH_DELAY_DEFAULT;

    if (!strchr(buff, ',')) {
        this->baudSpeed = atoi(buff);
        return;
    }

    unsigned int step {0};
    char* nextToken = nullptr;
    char* token = strtok_r(buff, ",", &nextToken);
    while (token != nullptr) {
        switch (step++) {
			case ParseDelayT::PARSE_BAUDRATE:
				this->baudSpeed = atoi(token);
				break;
			case ParseDelayT::PARSE_REQUEST_DELAY:
				this->requestDelay.delay = (atoi(token) < REQUEST_DELAY_DEFAULT ? REQUEST_DELAY_DEFAULT : atoi(token));
				break;
			case ParseDelayT::PARSE_MODBUS_DELAY:
				this->modbusDelay.delay = (atoi(token) < MODBUS_DELAY_DEFAULT ? MODBUS_DELAY_DEFAULT : atoi(token));
				break;
			case ParseDelayT::PARSE_PUSH_DELAY:
				this->pushDelay.delay = (atoi(token) < PUSH_DELAY_DEFAULT ? PUSH_DELAY_DEFAULT : atoi(token));
				break;
			default:
				break;
        }
        token = strtok_r(nullptr, ",", &nextToken);
    }
}

void MVPApplication::processParseConfigTotalRow(char* ptr, size_t len)
{

}

void MVPApplication::processParseConfigSensorDelay(char* ptr, size_t len)
{
    bool newSensor {true};
    char buff[len + 1] {0};
    size_t buffLen {0};

    this->sensorDelay.clear();

    SensorDelay_t sensor {};
    for (int i = 0; i < len; ++i) { 
        buff[buffLen++] = ptr[i];

        if (ptr[i] == ',') {
            buff[--buffLen] = '\0';
            buffLen = 0;

            if (newSensor) {
                newSensor = false;
                sensor = SensorDelay_t();
                sensor.address = atoi(buff);
            }
            else {
                newSensor = true;
                sensor.delay = atoi(buff);
                this->sensorDelay.put(sensor);
                if (this->sensorCount++ > MAXIMUM_MODBUS_DEVICES) {
                    break;
                }
            }
        }
    }
}

void MVPApplication::processParseConfigSensorReadWrite(char* ptr, size_t len)
{
    size_t position {0};

    this->modbusConfigParam.clear();

    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            ModbusConfig_t config {};
            this->parseOneConfigSensorReadWrite(ptr + position, i - position, config);
            position = i + 1;
            this->modbusConfigParam.put(config);
            if (this->readConfigCount++ > MAXIMUM_MODBUS_DEVICES) {
                break;
            }
        }
    }
}

void MVPApplication::parseOneConfigSensorReadWrite(char* ptr, size_t len, ModbusConfig_t& config)
{
    char buff[len + 1] {0};
    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};

    for (int i = 0; i < len; ++i) {
        buff[position++] = ptr[i];
        if (ptr[i] == ',') {
            buff[--position] = '\0';
            configParam[configIndex++] = atoi(buff);
            position = 0;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneConfigSensorReadWrite(configParam, configIndex, config);
}

void MVPApplication::actOneConfigSensorReadWrite(int* ptr, size_t len, ModbusConfig_t& config)
{
    if (ptr == nullptr)
        return;
    if (!len)
        return;

    config.addr = ptr[0];
    config.id = ptr[1];
    config.func = ptr[2];
    config.sa1 = ptr[3];
    config.sa2 = ptr[4];
    config.len1 = ptr[5];
    config.len2 = ptr[6];

    for (int i = 0; i < len - 7; ++i) {
        config.extra[i] = ptr[i + 7];
    }
}

void MVPApplication::processParseConfigLocalAlarm(char* ptr, size_t len)
{
    size_t position = 0;

    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            this->parseOneConfigLocalAlarm(ptr + position, i - position);
            position = i + 1;
        }
    }
}

void MVPApplication::parseOneConfigLocalAlarm(char* ptr, size_t len)
{

}

void MVPApplication::processParseStationContactPhone(char* ptr, size_t len)
{
    size_t position = 0;

    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            this->parseOneConfigPhoneNumber(ptr + position, i - position);
            position = i + 1;
        }
    }
}

void MVPApplication::parseOneConfigPhoneNumber(char* ptr, size_t len)
{

}

void MVPApplication::processParseIsWifi(char* ptr, size_t len)
{
    this->isWifi = (ptr[0] == '1') ? true : false;
}

void MVPApplication::processParseConfigWifi(char* ptr, size_t len)
{
    bool isSsid {true};

    memset(this->ssid, 0, sizeof(this->ssid));
    memset(this->password, 0, sizeof(this->password));

    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == ',') {
            isSsid = false;
            continue;
        }
        if (isSsid) {
            sprintf(&this->ssid[strlen(this->ssid)], "%c", ptr[i]);
        }
        else {
            sprintf(&this->password[strlen(this->password)], "%c", ptr[i]);
        }
    }
}

void MVPApplication::processParseConfigAliasTotalRow(char* ptr, size_t len)
{

}

void MVPApplication::processParseConfigAliasData(char* ptr, size_t len)
{
    size_t position {0};

    this->modbusConfigAliasParam.clear();
    
    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '.') {
            ModbusConfigAlias_t config {};
            this->processOneConfigAlias(ptr + position, i - position, config);
            position = i + 1;
            this->modbusConfigAliasParam.put(config);
            if (this->readConfigAliasCount++ > MAXIMUM_MODBUS_DEVICES) {
                break;
            }
        }
    }
}

void MVPApplication::processOneConfigAlias(char* ptr, size_t len, ModbusConfigAlias_t& config)
{
    if (len < 2)
        return;

    size_t position {0};

    for (int i = 0; i < len - 1; ++i) {
        if (ptr[i] == ',' && ptr[i + 1] == '-') {
            position = i + 1;
            this->parseOneConfigAlias(ptr, i + 1, config);
            this->processParseAction(ptr + position, len - position, config);
            break;
        }
    }
}

void MVPApplication::parseOneConfigAlias(char* ptr, size_t len, ModbusConfigAlias_t& config)
{
    if (len < 36)
        return;

    char buff[len + 1] {0};
    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};
    bool isAlias {false};
    char key[37] {0};

    for (int i = 0; i < len; ++i) {
        buff[position++] = ptr[i];
        if (ptr[i] == ',') {
            buff[--position] = '\0';
            if (isAlias) {
                memcpy(key, buff, 36);
            }
            else {
                configParam[configIndex++] = atoi(buff);
            }
            position = 0;
            isAlias = true;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneConfigAlias(configParam, configIndex, config, key);
}

void MVPApplication::actOneConfigAlias(int* ptr, size_t len, ModbusConfigAlias_t& config, const char* key)
{
    if (strlen(key) != 36)
        return;
    config.id = ptr[0];
    memcpy(config.key, key, 36);
}

void MVPApplication::processParseAction(char* ptr, size_t len, ModbusConfigAlias_t& config)
{
    size_t position {0};
    size_t numHyphen {0};

    config.readActionCount = 0;

    for (int i = 0; i < len; ++i) {
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

void MVPApplication::parseOneAction(char* ptr, size_t len, ModbusConfigAlias_t& config)
{
    char buff[len + 1] {0};
    size_t position {0};
    size_t configIndex {0};
    int configParam[20] {0};

    for (int i = 0; i < len; ++i) {
        buff[position++] = ptr[i];
        if (ptr[i] == ',') {
            buff[--position] = '\0';
            configParam[configIndex++] = atoi(buff);
            position = 0;
            if (configIndex >= 20) {
                break;
            }
        }
    }
    this->actOneAction(configParam, configIndex, config);
}

void MVPApplication::actOneAction(int* ptr, size_t len, ModbusConfigAlias_t& config)
{
    Action_t& action = config.action[config.readActionCount];

    action.id = ptr[0];
    action.len1 = ptr[1];
    action.len2 = ptr[2];
    for (int i = 0; i < len - 3; ++i) {
        action.extra[i] = ptr[i + 3];
    }
}

void MVPApplication::processParseIsEnableBluetooth(char* ptr, size_t len)
{
    this->isBluetooth = (ptr[0] == '1') ? true : false;
}
