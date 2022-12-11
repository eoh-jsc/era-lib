#ifndef INC_ERA_API_HPP_
#define INC_ERA_API_HPP_

#include <stdint.h>
#include <ERa/ERaPin.hpp>
#include <ERa/ERaPinDef.hpp>
#include <ERa/ERaData.hpp>
#include <ERa/ERaParam.hpp>
#include <Utility/ERaQueue.hpp>
#include <Modbus/ERaModbusSimple.hpp>
#include <Zigbee/ERaZigbeeSimple.hpp>

#if defined(analogInputToDigitalPin)
	#define ERA_DECODE_PIN(pin)		analogInputToDigitalPin(pin)
#else
	#define ERA_DECODE_PIN(pin)		pin
#endif

#define ERA_DECODE_PIN_NUMBER(pin)	pin
#define ERA_DECODE_PIN_NAME(pin) 	(((pin[0] == 'a') || (pin[0] == 'A')) ? 	\
									ERA_DECODE_PIN(atoi(pin + 1)) : ERA_DECODE_PIN_NUMBER(atoi(pin)))

typedef struct __ERaRsp_t {
    uint8_t type;
    ERaParam id;
    ERaParam param;
} ERaRsp_t;

class Internal {};

template <class Proto, class Flash>
class ERaApi
	: public Internal
#if defined(ERA_MODBUS)
	, public ERaModbus< ERaApi<Proto, Flash> >
#endif
#if defined(ERA_ZIGBEE)
	, public ERaZigbee< ERaApi<Proto, Flash> >
#endif
{
protected:
	enum ERaTypeRspT {
		ERA_RESPONSE_VIRTUAL_PIN = 0,
		ERA_RESPONSE_VIRTUAL_PIN_MULTI = 1,
		ERA_RESPONSE_DIGITAL_PIN = 2,
		ERA_RESPONSE_ANALOG_PIN = 3,
		ERA_RESPONSE_PWM_PIN = 4,
		ERA_RESPONSE_CONFIG_ID = 5,
		ERA_RESPONSE_CONFIG_ID_MULTI = 6,
		ERA_RESPONSE_MODBUS_DATA = 7,
		ERA_RESPONSE_ZIGBEE_DATA = 8
	};

private:
	typedef std::function<void(void*)> ReportPinCallback_t;

	ERaReport eraReport;

protected:
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
    {}
    ~ERaApi()
    {}

	template <typename T>
	void virtualWrite(int pin, T value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_VIRTUAL_PIN;
		rsp.id = pin;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	template <typename T, typename... Args>
	void virtualWrite(int pin, T value, Args... tail) {
		this->virtualMultiWrite(pin, value, tail...);
	}

	void digitalWrite(int pin, bool value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_DIGITAL_PIN;
		rsp.id = pin;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	void analogWrite(int pin, int value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_ANALOG_PIN;
		rsp.id = pin;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	void pwmWrite(int pin, int value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_PWM_PIN;
		rsp.id = pin;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	template <typename T>
	void configIdWrite(int configId, T value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_CONFIG_ID;
		rsp.id = configId;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	template <typename T, typename... Args>
	void configIdWrite(int configId, T value, Args... tail) {
		this->configIdMultiWrite(configId, value, tail...);
	}

	void modbusDataWrite(ERaDataBuff* value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_MODBUS_DATA;
		rsp.id = 0;
		rsp.param = 0;
		this->thisProto().sendCommand(rsp, value);
	}

	void zigbeeDataWrite(const char* id, cJSON* value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_ZIGBEE_DATA;
		rsp.id = id;
		rsp.param = value;
		this->thisProto().sendCommand(rsp);
	}

	void osStarts() {
#if defined(ERA_RTOS) && !defined(ERA_NO_RTOS)
		ERaOs::osStartsScheduler();
#endif
	}

protected:
	void addInfo(cJSON* root);
	void addModbusInfo(cJSON* root);
	void handleReadPin(cJSON* root);
	void handleWritePin(cJSON* root);
	void handlePinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);

	void begin() {
#if !defined(ERA_ASK_CONFIG_WHEN_RESTART)
		if (!this->thisProto().transp.getAskConfig()) {
			this->flash.begin();
		}
#endif
	}

	bool run() {
		ERA_RUN_YIELD();
		this->eraPinReport.run();
        return this->thisProto().transp.run();
	}

	void callERaWriteHandler(uint8_t pin, const ERaParam& param) {
		ERaWriteHandler handle = getERaWriteHandler(pin);
		if (handle != nullptr) {
			handle(pin, param);
		}
	}

	void callERaPinWriteHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) {
		ERaPinWriteHandler handle = getERaPinWriteHandler(pin);
		if (handle != nullptr) {
			handle(pin, param, raw);
		}
	}

	void callERaPinReadHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) {
		ERaPinReadHandler handle = getERaPinReadHandler(pin);
		if (handle != nullptr) {
			handle(pin, param, raw);
		}
	}

	void writeToFlash(const char ERA_UNUSED *filename, const char ERA_UNUSED *buf) {
#if !defined(ERA_ASK_CONFIG_WHEN_RESTART)
		if (!this->thisProto().transp.getAskConfig()) {
			this->flash.writeFlash(filename, buf);
		}
#endif
	}

	char* readFromFlash(const char ERA_UNUSED *filename) {
		char* buf = nullptr;
#if !defined(ERA_ASK_CONFIG_WHEN_RESTART)
		if (!this->thisProto().transp.getAskConfig()) {
			buf = this->flash.readFlash(filename);
		}
#endif
		return buf;
	}

	void removeFromFlash(const char ERA_UNUSED *filename) {
#if !defined(ERA_ASK_CONFIG_WHEN_RESTART)
		if (!this->thisProto().transp.getAskConfig()) {
			this->flash.writeFlash(filename, "");
		}
#endif
	}

	Flash& flash;
	ERaPin<ERaReport> eraPinReport {eraReport};

private:
	template <typename... Args>
	void virtualMultiWrite(Args... tail) {
		ERaRsp_t rsp;
		ERaDataJson data;
		data.add_multi(tail...);
		rsp.type = ERaTypeRspT::ERA_RESPONSE_VIRTUAL_PIN_MULTI;
		rsp.id = 0;
		rsp.param = 0;
		this->thisProto().sendCommand(rsp, &data);
	}

	template <typename... Args>
	void configIdMultiWrite(Args... tail) {
		ERaRsp_t rsp;
		ERaDataJson data;
		data.add_multi(tail...);
		rsp.type = ERaTypeRspT::ERA_RESPONSE_CONFIG_ID_MULTI;
		rsp.id = 0;
		rsp.param = data.getJson();
		this->thisProto().sendCommand(rsp);
	}

	void getScaleConfig(const cJSON* const root, PinConfig_t& pin);
	void getReportConfig(const cJSON* const root, PinConfig_t& pin);
	void getPinConfig(const cJSON* const root, PinConfig_t& pin);
	uint8_t getPinMode(const cJSON* const root, const uint8_t defaultMode = VIRTUAL);
	bool isReadPinMode(uint8_t pMode);
	bool getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin);
	bool isDigit(const std::string& str);

	void sendPinEvent(void* args) {
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
			case INPUT_PULLDOWN:
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

	void sendPinConfigEvent(void* args) {
		ERaReport::ReportData_t* rp = (ERaReport::ReportData_t*)args;
		if (rp == nullptr) {
			return;
		}
		ERaParam raw(rp->value);
		ERaParam param(rp->value);
		if (rp->scale.enable) {
			raw = ERaMapNumberRange(rp->value, rp->scale.min, rp->scale.max,
											rp->scale.rawMin, rp->scale.rawMax);
		}
		this->configIdWrite(rp->configId, rp->value);
		this->callERaPinReadHandler(rp->pin, param, raw);
	}

	inline
	const Proto& thisProto() const {
		return static_cast<const Proto&>(*this);
	}

	inline
	Proto& thisProto() {
		return static_cast<Proto&>(*this);
	}

	ReportPinCallback_t reportPinCb = [=](void* args) {
		this->sendPinEvent(args);
	};

	ReportPinCallback_t reportPinConfigCb = [=](void* args) {
		this->sendPinConfigEvent(args);
	};
};

template <class Proto, class Flash>
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

	return mode;
}

template <class Proto, class Flash>
bool ERaApi<Proto, Flash>::isReadPinMode(uint8_t pMode) {
	switch (pMode) {
		case INPUT:
		case INPUT_PULLUP:
		case INPUT_PULLDOWN:
		case ANALOG:
			return true;
		default:
			return false;
	}
}

template <class Proto, class Flash>
bool ERaApi<Proto, Flash>::getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin) {
	if (root == nullptr || key == nullptr) {
		return false;
	}

	if (strncmp(root->string, key, strlen(key))) {
		return false;
	}

	char* ptr = strstr(root->string, key);

	if (strchr((ptr += strlen(key)), '_')) {
		std::string str(strchr(ptr, '_') + 1);
		if (!str.empty()) {
			pin = ERA_DECODE_PIN_NAME(str.c_str());
		}
	}

	return true;
}

template <class Proto, class Flash>
bool ERaApi<Proto, Flash>::isDigit(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    size_t position {0};
    if (str.at(0) == '-' || str.at(0) == '+') {
        position = 1;
        if (str.size() < 2) {
            return false;
        }
    }
    for (auto i = str.begin() + position; i != str.end(); ++i) {
        if (!isdigit(*i)) {
            return false;
        }
    }
    return true;
}

#endif /* INC_ERA_API_HPP_ */
