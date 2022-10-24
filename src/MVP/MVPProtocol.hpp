#ifndef INC_MVP_PROTOCOL_HPP_
#define INC_MVP_PROTOCOL_HPP_

#include <vector>
#include <string>
#include <Utility/MVPFlashDef.hpp>
#include <MVP/MVPConfig.hpp>
#include <MVP/MVPHandlers.hpp>
#include <MVP/MVPApi.hpp>
#include <MVP/MVPTask.hpp>
#include <PnP/MVPState.hpp>

using namespace std;

template <class Transp, class Flash>
class MVPProto
	: public MVPApi< MVPProto<Transp, Flash>, Flash >
{
	enum MVPChipCfgT {
		CHIP_UPDATE_CONFIG = 0,
		CHIP_UPDATE_CONTROL = 1,
		CHIP_UPDATE_BLUETOOTH = 2,
		CHIP_SCAN_MODBUS = 3,
		CHIP_SEND_SMS = 4,
		CHIP_CONTROL_ALIAS = 5,
		CHIP_ARDUINO_PIN = 6
	};
	typedef void* ApiData_t;
    typedef std::function<void(String&, String&)> MessageCallback_t;

    char MVP_TOPIC[MAX_TOPIC_LENGTH];
	const char* TAG = "Protocol";
    const char* FILENAME_CONFIG = FILENAME_PIN_CONFIG;
    const char* BASE_TOPIC = "eoh/chip";
    const char* MVP_AUTH;
    friend class MVPApi< MVPProto<Transp, Flash>, Flash >;
    typedef MVPApi< MVPProto<Transp, Flash>, Flash > Base;

public:
    MVPProto(Transp& _transp, Flash& _flash)
        : Base(_flash)
		, transp(_transp)
    {
        memset(this->MVP_TOPIC, 0, sizeof(this->MVP_TOPIC));
    }
    ~MVPProto()
    {}
    
	void init() {
		Base::flash.begin();
		this->initPinConfig();
		this->initMVPTask();
	}

    void begin(const char* auth) {
		this->MVP_AUTH = auth;
        FormatString(this->MVP_TOPIC, "%s/%s", BASE_TOPIC, auth);
        this->transp.setAuth(auth);
		this->transp.setTopic(this->MVP_TOPIC);
		this->transp.onMessage(this->messageCb);
    }

    bool connect() {
        if (!this->transp.connect()) {
			return false;
		}
		this->printBanner();
		this->sendInfo();
		mvpOnConnected();
		return true;
    }

	void run() {
        this->transp.run();
		this->runMVPTask();
		Base::mvpPinReport.run();
	}

	void processRequest(std::string topic, const std::string payload) {
		MVP_LOG(TAG, "MQTT message: %s %s", topic.c_str(), payload.c_str());
		
		if (topic.find(this->MVP_TOPIC) != 0) {
			return;
		}

		topic.erase(0, strlen(this->MVP_TOPIC));
		std::vector<std::string> arrayTopic;
		this->splitString(topic, arrayTopic, '/');

		if (arrayTopic.size() < 2) {
			return;
		}

		if (arrayTopic.at(1) == "pin") {
			this->processPinRequest(arrayTopic, payload);
		}
		else if (arrayTopic.at(1) == "arduino_pin") {
			Base::processArduinoPinRequest(arrayTopic, payload);
		}
		else if (arrayTopic.at(1) == "down") {
			this->processDownRequest(arrayTopic, payload);
		}
	}

protected:
	Transp& transp;

private:
	void initMVPTask();
	void runMVPTask();
	void printBanner();
	void processPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processDownRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processDownAction(cJSON* root, cJSON* item);
	void processActionChip(cJSON* root, uint8_t type);
	void processDownCommand(cJSON* root, cJSON* item);
	void processFinalize(cJSON* root);
	void processConfiguration(cJSON* root);
	void processDeviceConfig(cJSON* root, uint8_t type);
	void processArduinoPin(cJSON* root);
	bool sendInfo();
	bool sendData(MVPRsp_t& rsp);
	bool sendPinData(MVPRsp_t& rsp);
	bool sendConfigIdData(MVPRsp_t& rsp);
	bool sendConfigIdMultiData(MVPRsp_t& rsp);
	bool sendModbusData(MVPRsp_t& rsp);
	bool sendZigbeeData(MVPRsp_t& rsp);
	void sendCommand(MVPRsp_t& rsp, ApiData_t data = nullptr);
	void sendCommandConfigIdMulti(MVPRsp_t& rsp, MVPDataBuff* data);
	void sendCommandModbus(MVPRsp_t& rsp, MVPDataBuff* data);
	void initPinConfig();
	void parsePinConfig(const char* str);
	void storePinConfig(const cJSON* const root);
	void removePinConfig();
	unsigned int splitString(const std::string& strInput, std::vector<std::string>& retList, char ch);

	MessageCallback_t messageCb = [=](String& topic, String& payload) {
		this->processRequest(topic.c_str(), payload.c_str());
	};
};

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::printBanner() {
	MVP_LOG(TAG, MVP_NEWLINE
		"    ____    ____    ________" MVP_NEWLINE
		"   /    \\  /    \\  /     _  )" MVP_NEWLINE
		"  /      \\/      \\/   /____/" MVP_NEWLINE
		" /   /\\      /\\      /" MVP_NEWLINE
		"/___/  \\____/  \\____/" MVP_NEWLINE
		MVP_NEWLINE
		"Connection successful!!!" MVP_NEWLINE
	);
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	if (arrayTopic.at(2) == "down") {
		Base::handlePinRequest(arrayTopic, payload);
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processDownRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}
	
	cJSON* item = cJSON_GetObjectItem(root, "action");
	if (cJSON_IsString(item)) {
		this->processDownAction(root, item);
	}
	item = cJSON_GetObjectItem(root, "command");
	if (cJSON_IsString(item)) {
		this->processDownCommand(root, item);
	}
	
	cJSON_Delete(root);
	root = nullptr;
	item = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processDownAction(cJSON* root, cJSON* item) {
	if (MVPStrCmp(item->valuestring, "update_configuration")) {
		processActionChip(root, MVPChipCfgT::CHIP_UPDATE_CONFIG);
	}
	else if (MVPStrCmp(item->valuestring, "send_control")) {
		processActionChip(root, MVPChipCfgT::CHIP_UPDATE_CONTROL);
	}
	else if (MVPStrCmp(item->valuestring, "send_command")) {
		processActionChip(root, MVPChipCfgT::CHIP_CONTROL_ALIAS);
	}
	else if (MVPStrCmp(item->valuestring, "update_firmware")) {
		MVPState::set(StateT::STATE_OTA_UPGRADE);
	}
	else if (MVPStrCmp(item->valuestring, "reset_eeprom")) {
#if defined(MVP_MODBUS)
		Base::MVPModbus::removeConfigFromFlash();
#endif
		this->removePinConfig();
		MVPDelay(1000);
		MVPState::set(StateT::STATE_RESET_CONFIG_REBOOT);
	}
	else if (MVPStrCmp(item->valuestring, "force_reset")) {
		MVPRestart(false);
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processActionChip(cJSON* root, uint8_t type) {
	cJSON* dataItem = cJSON_GetObjectItem(root, "data");
	if (!cJSON_IsObject(dataItem)) {
		return;
	}
	cJSON* item = nullptr;
	switch (type) {
		case MVPChipCfgT::CHIP_UPDATE_CONFIG:
			item = cJSON_GetObjectItem(dataItem, "configuration");
			if (cJSON_IsString(item)) {
#if defined(MVP_MODBUS)
				Base::MVPModbus::parseModbusConfig(item->valuestring, false);
				Base::MVPModbus::clearDataBuff();
#endif
			}
			break;
		case MVPChipCfgT::CHIP_UPDATE_CONTROL:
			item = cJSON_GetObjectItem(dataItem, "control");
			if (cJSON_IsString(item)) {
#if defined(MVP_MODBUS)
				Base::MVPModbus::parseModbusConfig(item->valuestring, true);
#endif
			}
			break;
		case MVPChipCfgT::CHIP_CONTROL_ALIAS:
			item = cJSON_GetObjectItem(dataItem, "commands");
			if (cJSON_IsArray(item)) {
				cJSON* keyItem = nullptr;
				for(int i = 0; i < cJSON_GetArraySize(item); ++i) {
					keyItem = cJSON_GetArrayItem(item, i);
					if(cJSON_IsString(keyItem)) {
						// Handle command
#if defined(MVP_MODBUS)
						Base::MVPModbus::addModbusAction(keyItem->valuestring);
#endif
					}
				}
			}
			break;
		default:
			break;
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processDownCommand(cJSON* root, cJSON* item) {
	if (MVPStrCmp(item->valuestring, "finalize_configuration")) {
		this->processFinalize(root);
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processFinalize(cJSON* root) {
	cJSON* item = cJSON_GetObjectItem(root, "configuration");
	if (cJSON_IsObject(item)) {
		this->processConfiguration(item);
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processConfiguration(cJSON* root) {
	cJSON* item = cJSON_GetObjectItem(root, "arduino_pin");
	if (cJSON_IsObject(item)) {
		Base::mvpPinReport.deleteAll();
		this->processDeviceConfig(item, MVPChipCfgT::CHIP_ARDUINO_PIN);
		this->storePinConfig(item);
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processDeviceConfig(cJSON* root, uint8_t type) {
	cJSON* item = cJSON_GetObjectItem(root, "devices");
	if (!cJSON_IsArray(item)) {
		return;
	}
	cJSON* current = nullptr;

	for (current = item->child; current != nullptr; current = current->next) {
		switch (type) {
			case MVPChipCfgT::CHIP_ARDUINO_PIN:
				this->processArduinoPin(current);
				break;
			default:
				break;
		}
	}
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::processArduinoPin(cJSON* root) {
	if (!cJSON_IsObject(root)) {
		return;
	}
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
		if (MVPStrCmp(current->string, "read_pins")) {
			Base::handleReadPin(current);
		}
		if (MVPStrCmp(current->string, "control_pins")) {
			Base::handleWritePin(current);
		}
	}
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendInfo() {
	bool status {false};
	cJSON* root = cJSON_CreateObject();
	if(root == nullptr) {
		return false;
	}

	Base::addInfo(root);

    char topic[MAX_TOPIC_LENGTH] {0};
	char* payload = cJSON_PrintUnformatted(root);
    FormatString(topic, this->MVP_TOPIC);
    FormatString(topic, "/info");
	
	if (payload != nullptr) {
		status = this->transp.publishData(topic, payload);
	}

	cJSON_Delete(root);
	free(payload);
	root = nullptr;
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendData(MVPRsp_t& rsp) {
	switch (rsp.type) {
		case Base::MVPTypeRspT::MVP_RESPONSE_VIRTUAL_PIN:
		case Base::MVPTypeRspT::MVP_RESPONSE_DIGITAL_PIN:
		case Base::MVPTypeRspT::MVP_RESPONSE_ANALOG_PIN:
		case Base::MVPTypeRspT::MVP_RESPONSE_PWM_PIN:
		case Base::MVPTypeRspT::MVP_RESPONSE_INTERRUPT_PIN:
			return this->sendPinData(rsp);
		case Base::MVPTypeRspT::MVP_RESPONSE_CONFIG_ID:
			return this->sendConfigIdData(rsp);
		case Base::MVPTypeRspT::MVP_RESPONSE_CONFIG_ID_MULTI:
			return this->sendConfigIdMultiData(rsp);
		case Base::MVPTypeRspT::MVP_RESPONSE_MODBUS_DATA:
			return this->sendModbusData(rsp);
		case Base::MVPTypeRspT::MVP_RESPONSE_ZIGBEE_DATA:
			return this->sendZigbeeData(rsp);
		default:
			return false;
	}
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendPinData(MVPRsp_t& rsp) {
	char name[50] {0};
	bool status {false};
	char* payload = nullptr;
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->MVP_TOPIC);
	FormatString(topicName, "/pin/data");
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return false;
	}
	switch (rsp.type) {
	case Base::MVPTypeRspT::MVP_RESPONSE_VIRTUAL_PIN:
		FormatString(name, "virtual_pin_%d", (int)rsp.id);
		break;
	case Base::MVPTypeRspT::MVP_RESPONSE_DIGITAL_PIN:
		FormatString(name, "digital_pin_%d", (int)rsp.id);
		break;
	case Base::MVPTypeRspT::MVP_RESPONSE_ANALOG_PIN:
		FormatString(name, "analog_pin_%d", (int)rsp.id);
		break;
	case Base::MVPTypeRspT::MVP_RESPONSE_PWM_PIN:
		FormatString(name, "pwm_pin_%d", (int)rsp.id);
		break;
	case Base::MVPTypeRspT::MVP_RESPONSE_INTERRUPT_PIN:
		FormatString(name, "interrupt_pin_%d", (int)rsp.id);
		break;
	default:
		cJSON_Delete(root);
		root = nullptr;
		return false;
	}
	cJSON_AddNumberToObject(root, name, rsp.param);
	payload = cJSON_PrintUnformatted(root);
	if (payload != nullptr) {
		status = this->transp.publishData(topicName, payload);
	}
	cJSON_Delete(root);
	free(payload);
	root = nullptr;
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendConfigIdData(MVPRsp_t& rsp) {
	bool status {false};
	char* payload = nullptr;
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->MVP_TOPIC);
	FormatString(topicName, "/config/%d/value", (int)rsp.id);
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return false;
	}
	cJSON_AddNumberToObject(root, "v", rsp.param);
	payload = cJSON_PrintUnformatted(root);
	if (payload != nullptr) {
		status = this->transp.publishData(topicName, payload);
	}
	cJSON_Delete(root);
	free(payload);
	root = nullptr;
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendConfigIdMultiData(MVPRsp_t& rsp) {
	bool status {false};
	char* payload = rsp.param;
	if (payload == nullptr) {
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->MVP_TOPIC);
	FormatString(topicName, "/config_value");
	status = this->transp.publishData(topicName, payload);
	free(payload);
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendModbusData(MVPRsp_t& rsp) {
	bool status {false};
	char* payload = rsp.param;
	if (payload == nullptr) {
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->MVP_TOPIC);
	FormatString(topicName, "/data");
	status = this->transp.publishData(topicName, payload);
	free(payload);
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool MVPProto<Transp, Flash>::sendZigbeeData(MVPRsp_t& rsp) {
	bool status {false};
	char* topic = rsp.id;
	char* payload = rsp.param;
	if (topic == nullptr || payload == nullptr) {
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->MVP_TOPIC);
	FormatString(topicName, topic);
	status = this->transp.publishData(topicName, payload);
	free(topic);
	free(payload);
	topic = nullptr;
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::sendCommand(MVPRsp_t& rsp, ApiData_t data) {
	switch (rsp.type) {
		case Base::MVPTypeRspT::MVP_RESPONSE_CONFIG_ID_MULTI: {
			MVPDataBuff* configIdData = (MVPDataBuff*)data;
			this->sendCommandConfigIdMulti(rsp, configIdData);
		}
			break;
		case Base::MVPTypeRspT::MVP_RESPONSE_MODBUS_DATA: {
			MVPDataBuff* modbusData = (MVPDataBuff*)data;
			this->sendCommandModbus(rsp, modbusData);
		}
			break;
		default:
			break;
	}
	this->sendData(rsp);
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::sendCommandConfigIdMulti(MVPRsp_t& rsp, MVPDataBuff* data) {
	if (data == nullptr) {
		return;
	}
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return;
	}
	const MVPDataBuff::iterator e = data->end();
	for (MVPDataBuff::iterator it = data->begin(); it < e; ++it) {
		const char* name = it;
		++it;
		if (!it.isEmpty()) {
			cJSON_AddNumberToObject(root, name, it.getFloat());
		}
	}
	rsp.param = root;
	cJSON_Delete(root);
	root = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::sendCommandModbus(MVPRsp_t& rsp, MVPDataBuff* data) {
	if (data == nullptr) {
		return;
	}
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return;
	}
	int mbFailed {0};
	int mbTotal {0};
	size_t index {0};
	size_t dataLen = data->getLen() + 1;
	char* mbData = (char*)MVP_CALLOC(dataLen, sizeof(char));
	if (mbData == nullptr) {
		return;
	}
	char* mbAck = (char*)MVP_CALLOC(dataLen, sizeof(char));
	if (mbAck == nullptr) {
		return;
	}
	const MVPDataBuff::iterator e = data->end();
	for (MVPDataBuff::iterator it = data->begin(); it < e; ++it) {
		if (it == "fail") {
			++it;
			mbFailed = it.getInt(16);
		}
		else if (it == "total") {
			++it;
			mbTotal = it.getInt(16);
		}
		else {
			if (index++ % 2 == 0) {
				FormatString(mbData, dataLen, it);
			}
			else {
				FormatString(mbAck, dataLen, it);
			}
		}
	}
	cJSON_AddStringToObject(root, "data", mbData);
	cJSON_AddStringToObject(root, "ack", mbAck);
	cJSON_AddNumberToObject(root, "modbus_fail", mbFailed);
	cJSON_AddNumberToObject(root, "modbus_total", mbTotal);
	Base::addModbusInfo(root);
	free(mbData);
	free(mbAck);
	mbData = nullptr;
	mbAck = nullptr;
	rsp.param = root;
	cJSON_Delete(root);
	root = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::initPinConfig() {
    char* ptr = nullptr;
    ptr = Base::readFromFlash(FILENAME_CONFIG);
    this->parsePinConfig(ptr);
    free(ptr);
	ptr = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::parsePinConfig(const char* str) {
	if (str == nullptr) {
		return;
	}
	cJSON* root = cJSON_Parse(str);
	if (cJSON_IsObject(root)) {
		this->processDeviceConfig(root, MVPChipCfgT::CHIP_ARDUINO_PIN);
	}
	cJSON_Delete(root);
	root = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::storePinConfig(const cJSON* const root) {
	if (root == nullptr) {
		return;
	}
	char* ptr = cJSON_PrintUnformatted(root);
	if (ptr != nullptr) {
		Base::writeToFlash(FILENAME_CONFIG, ptr);
	}
	free(ptr);
	ptr = nullptr;
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::removePinConfig() {
	Base::mvpPinReport.deleteAll();
	Base::removeFromFlash(FILENAME_CONFIG);
}

template <class Transp, class Flash>
unsigned int MVPProto<Transp, Flash>::splitString(const std::string& strInput, std::vector<std::string>& retList, char ch) {
	if (strInput.empty()) {
		return -1;
	}

	unsigned int pos = strInput.find(ch);
	unsigned int initPos = 0;
	retList.clear();

	while (pos != std::string::npos) {
		std::string str_sub = strInput.substr(initPos, pos - initPos);// initPos+1);
		retList.push_back(str_sub);
		initPos = pos + 1;
		pos = strInput.find(ch, initPos);
	}

	std::string lastSub = strInput.substr(initPos, std::min(pos, strInput.size()) - initPos + 1);
	retList.push_back(lastSub);
	return retList.size();
}

#endif /* INC_MVP_PROTOCOL_HPP_ */