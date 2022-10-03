#ifndef INC_MVP_API_HPP_
#define INC_MVP_API_HPP_

#include <stdint.h>
#include <MVP/MVPPin.hpp>
#include <MVP/MVPPinDef.hpp>
#include <MVP/MVPData.hpp>
#include <MVP/MVPParam.hpp>
#include <Utility/MVPQueue.hpp>
#include <Modbus/MVPModbusSimple.hpp>

#if defined(analogInputToDigitalPin)
	#define MVP_DECODE_PIN(pin)	analogInputToDigitalPin(pin)
#else
	#define MVP_DECODE_PIN(pin)	pin
#endif

typedef struct __MVPRsp_t {
    uint8_t type;
    MVPParam id;
    MVPParam param;
} MVPRsp_t;

template <class Proto, class Flash>
class MVPApi 
#if defined(MVP_MODBUS)
	: public MVPModbus< MVPApi<Proto, Flash> >
#endif
{
protected:
	enum MVPTypeRspT {
		MVP_RESPONSE_VIRTUAL_PIN = 0,
		MVP_RESPONSE_DIGITAL_PIN = 1,
		MVP_RESPONSE_ANALOG_PIN = 2,
		MVP_RESPONSE_PWM_PIN = 3,
		MVP_RESPONSE_INTERRUPT_PIN = 4,
		MVP_RESPONSE_CONFIG_ID = 5,
		MVP_RESPONSE_CONFIG_ID_MULTI = 6,
		MVP_RESPONSE_MODBUS_DATA = 7,
		MVP_RESPONSE_ZIGBEE_DATA = 8
	};

private:
	typedef std::function<void(void*)> ReportPinCallback_t;

	MVPReport mvpReport;

#if defined(MVP_MODBUS)
    friend class MVPModbus< MVPApi<Proto, Flash> >;
	typedef MVPModbus< MVPApi<Proto, Flash> > Modbus;
#endif
#if defined(MVP_ZIGBEE)
	friend class MVPZigbee< MVPApi<Proto, Flash> >;
	typedef MVPZigbee< MVPApi<Proto, Flash> > Zigbee;
#endif

public:
    MVPApi(Flash& _flash)
		: flash(_flash)
    {}
    ~MVPApi()
    {}

	template <typename T>
	void virtualWrite(int pin, T value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_VIRTUAL_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void digitalWrite(int pin, bool value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_DIGITAL_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void analogWrite(int pin, int value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_ANALOG_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void pwmWrite(int pin, int value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_PWM_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	void interruptWrite(int pin, bool value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_INTERRUPT_PIN;
		rsp.id = pin;
		rsp.param = value;
		static_cast<Proto*>(this)->sendCommand(rsp);
	}

	template <typename T>
	void configIdWrite(int configId, T value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_CONFIG_ID;
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
		MVPRsp_t rsp;
		char mem[MVP_BUFFER_SIZE] {0};
		MVPDataBuff data(mem, sizeof(mem));
		rsp.type = MVPTypeRspT::MVP_RESPONSE_CONFIG_ID_MULTI;
		rsp.id = 0;
		rsp.param = 0;
		data.add_multi(tail...);
		static_cast<Proto*>(this)->sendCommand(rsp, &data);
	}

	void modbusDataWrite(MVPDataBuff* value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_MODBUS_DATA;
		rsp.id = 0;
		rsp.param = 0;
		static_cast<Proto*>(this)->sendCommand(rsp, value);
	}

	void zigbeeDataWrite(const char* id, cJSON* value) {
		MVPRsp_t rsp;
		rsp.type = MVPTypeRspT::MVP_RESPONSE_ZIGBEE_DATA;
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

	void callMVPWriteHandler(uint8_t pin, const MVPParam& param) {
		MVPWriteHandler handle = getMVPWriteHandler(pin);
		if (handle != nullptr) {
			handle(pin, param);
		}
	}

	void callMVPPinWriteHandler(uint8_t pin, const MVPParam& param) {
		MVPPinWriteHandler handle = getMVPPinWriteHandler(pin);
		if (handle != nullptr) {
			handle(pin, param);
		}
	}

	void callMVPPinReadHandler(uint8_t pin, const MVPParam& param) {
		MVPPinReadHandler handle = getMVPPinReadHandler(pin);
		if (handle != nullptr) {
			handle(pin, param);
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
	MVPPin<MVPReport> mvpPinReport {mvpReport};

private:
	void getPinConfig(const cJSON* const root, PinConfig_t& pin);
	uint8_t getPinMode(const cJSON* const root, const uint8_t defaultMode);
	bool getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin);
	bool isDigit(const std::string& str);

	void sendPinEvent(void* args) {
		MVPReport::ReportData_t* rp = (MVPReport::ReportData_t*)args;
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
		MVPReport::ReportData_t* rp = (MVPReport::ReportData_t*)args;
		if (rp == nullptr) {
			return;
		}
		MVPParam param;
		param = rp->value;
		this->configIdWrite(rp->configId, rp->value);
		this->callMVPPinReadHandler(rp->pin, param);
	}

	ReportPinCallback_t reportPinCb = [=](void* args) {
		this->sendPinEvent(args);
	};

	ReportPinCallback_t reportPinConfigCb = [=](void* args) {
		this->sendPinConfigEvent(args);
	};
};

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::getPinConfig(const cJSON* const root, PinConfig_t& pin) {
	if (root == nullptr) {
		return;
	}
	cJSON* item = cJSON_GetObjectItem(root, "value_converter_configuration");
	if (!cJSON_IsObject(item)) {
		return;
	}

	cJSON* subItem = cJSON_GetObjectItem(item, "interval");
	if (cJSON_IsNumber(subItem)) {
		pin.report.interval = subItem->valueint;
	}
	subItem = cJSON_GetObjectItem(item, "min_interval");
	if (cJSON_IsNumber(subItem)) {
		pin.report.minInterval = subItem->valueint;
	}
	subItem = cJSON_GetObjectItem(item, "max_interval");
	if (cJSON_IsNumber(subItem)) {
		pin.report.maxInterval = subItem->valueint;
	}
	subItem = cJSON_GetObjectItem(item, "reportable_change");
	if (cJSON_IsNumber(subItem)) {
		pin.report.reportableChange = ((subItem->valuedouble) == 0 ? 1.0f : subItem->valuedouble);
	}
	subItem = cJSON_GetObjectItem(item, "channel");
	if (cJSON_IsNumber(subItem)) {
		pin.pwm.channel = subItem->valueint;
	}
	subItem = cJSON_GetObjectItem(item, "frequency");
	if (cJSON_IsNumber(subItem)) {
		pin.pwm.frequency = subItem->valueint;
	}
	subItem = cJSON_GetObjectItem(item, "resolution");
	if (cJSON_IsNumber(subItem)) {
		pin.pwm.resolution = subItem->valueint;
	}
}

template <class Proto, class Flash>
uint8_t MVPApi<Proto, Flash>::getPinMode(const cJSON* const root, const uint8_t defaultMode) {
	uint8_t mode = defaultMode;
	if (root == nullptr) {
		return mode;
	}
	cJSON* item = cJSON_GetObjectItem(root, "value_converter");
	if (!cJSON_IsString(item)) {
		return mode;
	}
	if (!strlen(item->valuestring)) {
		return mode;
	}

	if (MVPStrCmp(item->valuestring, "output")) {
		mode = OUTPUT;
	}
	else if (MVPStrCmp(item->valuestring, "open_drain")) {
		mode = OUTPUT_OPEN_DRAIN;
	}
	else if (MVPStrCmp(item->valuestring, "pwm")) {
		mode = PWM;
	}
	else if (MVPStrCmp(item->valuestring, "input")) {
		mode = INPUT;
	}
	else if (MVPStrCmp(item->valuestring, "pullup")) {
		mode = INPUT_PULLUP;
	}
	else if (MVPStrCmp(item->valuestring, "pulldown")) {
		mode = INPUT_PULLDOWN;
	}
	else if (MVPStrCmp(item->valuestring, "analog")) {
		mode = ANALOG;
	}

	return mode;
}

template <class Proto, class Flash>
bool MVPApi<Proto, Flash>::getGPIOPin(const cJSON* const root, const char* key, uint8_t& pin) {
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
			pin = isAnalog ? MVP_DECODE_PIN(atoi(str.c_str())) : atoi(str.c_str());
		}
	}

	return true;
}

template <class Proto, class Flash>
bool MVPApi<Proto, Flash>::isDigit(const std::string& str) {
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

#endif /* INC_MVP_API_HPP_ */