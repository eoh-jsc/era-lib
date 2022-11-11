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
	#define ERA_DECODE_PIN(pin)	analogInputToDigitalPin(pin)
#else
	#define ERA_DECODE_PIN(pin)	pin
#endif

typedef struct __ERaRsp_t {
    uint8_t type;
    ERaParam id;
    ERaParam param;
} ERaRsp_t;

template <class Proto, class Flash>
class ERaApi 
#if defined(ERA_MODBUS) && \
	defined(ERA_ZIGBEE)
	: public ERaModbus< ERaApi<Proto, Flash> >
	, public ERaZigbee< ERaApi<Proto, Flash> >
#elif defined(ERA_MODBUS) && \
	!defined(ERA_ZIGBEE)
	: public ERaModbus< ERaApi<Proto, Flash> >
#elif !defined(ERA_MODBUS) && \
	defined(ERA_ZIGBEE)
	: public ERaZigbee< ERaApi<Proto, Flash> >
#endif
{
protected:
	enum ERaTypeRspT {
		ERA_RESPONSE_VIRTUAL_PIN = 0,
		ERA_RESPONSE_DIGITAL_PIN = 1,
		ERA_RESPONSE_ANALOG_PIN = 2,
		ERA_RESPONSE_PWM_PIN = 3,
		ERA_RESPONSE_CONFIG_ID = 4,
		ERA_RESPONSE_CONFIG_ID_MULTI = 5,
		ERA_RESPONSE_MODBUS_DATA = 6,
		ERA_RESPONSE_ZIGBEE_DATA = 7
	};

private:
	typedef std::function<void(void*)> ReportPinCallback_t;

	ERaReport eraReport;

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
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void digitalWrite(int pin, bool value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_DIGITAL_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void analogWrite(int pin, int value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_ANALOG_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void pwmWrite(int pin, int value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_PWM_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	template <typename T>
	void configIdWrite(int configId, T value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_CONFIG_ID;
		rsp.id = configId;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	template <typename T, typename... Args>
	void configIdWrite(int configId, T value, Args... tail) {
		this->configIdWrite(configId, value);
		this->configIdWrite(tail...);
	}

	template <typename... Args>
	void configIdMultiWrite(Args... tail) {
		ERaRsp_t rsp;
		char mem[ERA_BUFFER_SIZE] {0};
		ERaDataBuff data(mem, sizeof(mem));
		rsp.type = ERaTypeRspT::ERA_RESPONSE_CONFIG_ID_MULTI;
		rsp.id = 0;
		rsp.param = 0;
		data.add_multi(tail...);
		static_cast<Proto*>(this)->sendCommand(rsp, &data);
	}

	void modbusDataWrite(ERaDataBuff* value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_MODBUS_DATA;
		rsp.id = 0;
		rsp.param = 0;
		static_cast<Proto*>(this)->sendCommand(rsp, value);
	}

	void zigbeeDataWrite(const char* id, cJSON* value) {
		ERaRsp_t rsp;
		rsp.type = ERaTypeRspT::ERA_RESPONSE_ZIGBEE_DATA;
		rsp.id = id;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

protected:
	void addInfo(cJSON* root);
	void addModbusInfo(cJSON* root);
	void handleReadPin(cJSON* root);
	void handleWritePin(cJSON* root);
	void handlePinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);

	void run() {
        static_cast<Proto*>(this)->transp.run();
		this->eraPinReport.run();
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

	void writeToFlash(const char* filename, const char* buf) {
		this->flash.writeFlash(filename, buf);
	}

	char* readFromFlash(const char* filename) {
		return this->flash.readFlash(filename);
	}

	void removeFromFlash(const char* filename) {
		this->flash.writeFlash(filename, "");
	}

	Flash& flash;
	ERaPin<ERaReport> eraPinReport {eraReport};

private:
	void getScaleConfig(const cJSON* const root, PinConfig_t& pin);
	void getReportConfig(const cJSON* const root, PinConfig_t& pin);
	void getPinConfig(const cJSON* const root, PinConfig_t& pin);
	uint8_t getPinMode(const cJSON* const root, const uint8_t defaultMode);
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
	cJSON* item = cJSON_GetObjectItem(root, "value_converter");
	if (item == nullptr) {
		item = cJSON_GetObjectItem(root, "pin_mode");
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
bool ERaApi<Proto, Flash>::getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin) {
	if (root == nullptr || key == nullptr) {
		return false;
	}

	if (strncmp(root->string, key, strlen(key))) {
		return false;
	}

	bool isAnalog {false};
	char* ptr = strstr(root->string, key);

	if (strchr((ptr += strlen(key)), '_'))	{
		std::string str(strchr(ptr, '_') + 1);
		if (!str.empty() && str.at(0) == 'A')	{
			str.erase(0, 1);
			isAnalog = true;
		}
		if (!str.empty() && this->isDigit(str)) {
			pin = isAnalog ? ERA_DECODE_PIN(atoi(str.c_str())) : atoi(str.c_str());
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
