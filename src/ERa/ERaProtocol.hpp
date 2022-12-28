#ifndef INC_ERA_PROTOCOL_HPP_
#define INC_ERA_PROTOCOL_HPP_

#if __has_include("ERaOptions.hpp")
    #include "ERaOptions.hpp"
#endif

#include <vector>
#include <string>
#include <Utility/ERaFlashDef.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaHandlers.hpp>
#include <ERa/ERaApi.hpp>
#include <ERa/ERaTask.hpp>
#include <PnP/ERaState.hpp>

using namespace std;

template <class Transp, class Flash>
class ERaProto
	: public ERaApi< ERaProto<Transp, Flash>, Flash >
{
	enum ERaChipCfgT {
		CHIP_UPDATE_CONFIG = 0,
		CHIP_UPDATE_CONTROL = 1,
		CHIP_UPDATE_BLUETOOTH = 2,
		CHIP_SCAN_MODBUS = 3,
		CHIP_SEND_SMS = 4,
		CHIP_CONTROL_ALIAS = 5,
		CHIP_IO_PIN = 6
	};
	typedef void* ApiData_t;
    typedef std::function<void(std::string&, std::string&)> MessageCallback_t;

    char ERA_TOPIC[MAX_TOPIC_LENGTH];
	const char* TAG = "Protocol";
    const char* FILENAME_CONFIG = FILENAME_PIN_CONFIG;
    const char* BASE_TOPIC = "eoh/chip";
    const char* ERA_AUTH;
    friend class ERaApi< ERaProto<Transp, Flash>, Flash >;
    typedef ERaApi< ERaProto<Transp, Flash>, Flash > Base;

public:
    ERaProto(Transp& _transp, Flash& _flash)
        : Base(_flash)
		, transp(_transp)
    {
        memset(this->ERA_TOPIC, 0, sizeof(this->ERA_TOPIC));
    }
    ~ERaProto()
    {}

    void begin(const char* auth) {
		this->ERA_AUTH = auth;
		ClearArray(this->ERA_TOPIC);
        FormatString(this->ERA_TOPIC, "%s/%s", BASE_TOPIC, auth);
        this->transp.setAuth(auth);
		this->transp.setTopic(this->ERA_TOPIC);
		this->transp.onMessage(this->messageCb);
    }

	void run() {
		if (!Base::run()) {
			ERaState::set(StateT::STATE_DISCONNECTED);
		}
		this->runERaTask();
	}

	void sendCommand(const char* auth, ERaRsp_t& rsp, ApiData_t data = nullptr);

	void syncConfig() {
		this->transp.syncConfig();
	}

	void askConfigWhenRestart(bool enable = true) {
		this->transp.setAskConfig(enable);
	}

protected:
	void init() {
		Base::begin();
		this->initPinConfig();
		this->initERaTask();
	}

    bool connect() {
        if (!this->transp.connect()) {
			return false;
		}
		this->printBanner();
		this->sendInfo();
		return true;
    }

	Transp& transp;

private:
	void initERaTask();
	void runERaTask();
	void printBanner();
	void processPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processDownRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processDownAction(cJSON* root, cJSON* item);
	void processActionChip(cJSON* root, uint8_t type);
	void processDownCommand(cJSON* root, cJSON* item);
	void processFinalize(cJSON* root);
	void processConfiguration(cJSON* root);
	void processDeviceConfig(cJSON* root, uint8_t type);
	void processIOPin(cJSON* root);
#if defined(ERA_ZIGBEE)
	void processZigbeeRequest(const std::vector<std::string>& arrayTopic, const std::string& payload);
	void processDeviceZigbee(const std::string& payload);
	void processActionDeviceZigbee(const cJSON* const root, uint8_t type);
	void processActionZigbee(const std::string& ieeeAddr, const std::string& payload, uint8_t type);
#endif
	bool sendInfo();
	bool sendData(ERaRsp_t& rsp);
	bool sendPinData(ERaRsp_t& rsp);
	bool sendConfigIdData(ERaRsp_t& rsp);
	bool sendConfigIdMultiData(ERaRsp_t& rsp);
	bool sendModbusData(ERaRsp_t& rsp);
	bool sendZigbeeData(ERaRsp_t& rsp);
	void sendCommandMulti(const char* auth, ERaRsp_t& rsp, ERaDataJson* data);
	void sendCommand(ERaRsp_t& rsp, ApiData_t data = nullptr);
	void sendCommandVirtual(ERaRsp_t& rsp, ERaDataJson* data);
	void sendCommandModbus(ERaRsp_t& rsp, ERaDataBuff* data);
	void initPinConfig();
	void parsePinConfig(const char* str);
	void storePinConfig(const cJSON* const root);
	void removePinConfig();
	void processRequest(std::string& topic, const std::string& payload);
	unsigned int splitString(const std::string& strInput, std::vector<std::string>& retList, char ch);

	MessageCallback_t messageCb = [=](std::string& topic, std::string& payload) {
		this->processRequest(topic, payload);
	};
};

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::printBanner() {
	ERA_LOG(TAG, ERA_PSTR(ERA_NEWLINE
		"  ____  ____         	 " ERA_NEWLINE
		" / _   / _  \\  _     	 " ERA_NEWLINE
		"/____ / / __/ /.\\    	 " ERA_NEWLINE
		" / _ /  _ \\  / _ \\ 	 " ERA_NEWLINE
		"/___/__//_/`/_/ \\_\\	 " ERA_NEWLINE
		"           (v" ERA_FIRMWARE_VERSION " for " ERA_BOARD_TYPE ")"
		ERA_NEWLINE
		"Connection successful (ping: %lums)!!!" ERA_NEWLINE),
		(unsigned long)this->transp.getPing()
	);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processRequest(std::string& topic, const std::string& payload) {
	ERA_LOG(this->transp.getTag(), ERA_PSTR("Message %s: %s"), topic.c_str(), payload.c_str());

	if (topic.find(this->ERA_TOPIC) != 0) {
		return;
	}

	topic.erase(0, strlen(this->ERA_TOPIC));
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
	else if (arrayTopic.at(1) == "virtual_pin") {
		Base::processVirtualPinRequest(arrayTopic, payload);
	}
	else if (arrayTopic.at(1) == "down") {
		this->processDownRequest(arrayTopic, payload);
	}
#if defined(ERA_ZIGBEE)
	else if (arrayTopic.at(1) == "zigbee") {
		this->processZigbeeRequest(arrayTopic, payload);
	}
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	if (arrayTopic.at(2) == "down") {
		Base::handlePinRequest(arrayTopic, payload);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDownRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
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
	ERA_FORCE_UNUSED(arrayTopic);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDownAction(cJSON* root, cJSON* item) {
	if (ERaStrCmp(item->valuestring, "update_configuration")) {
		processActionChip(root, ERaChipCfgT::CHIP_UPDATE_CONFIG);
	}
	else if (ERaStrCmp(item->valuestring, "send_control")) {
		processActionChip(root, ERaChipCfgT::CHIP_UPDATE_CONTROL);
	}
	else if (ERaStrCmp(item->valuestring, "send_command")) {
		processActionChip(root, ERaChipCfgT::CHIP_CONTROL_ALIAS);
	}
	else if (ERaStrCmp(item->valuestring, "update_firmware")) {
		ERaState::set(StateT::STATE_OTA_UPGRADE);
	}
	else if (ERaStrCmp(item->valuestring, "reset_eeprom")) {
#if defined(ERA_MODBUS)
		Base::ERaModbus::removeConfigFromFlash();
#endif
		this->removePinConfig();
		ERaDelay(1000);
		ERaState::set(StateT::STATE_RESET_CONFIG_REBOOT);
	}
	else if (ERaStrCmp(item->valuestring, "force_reset")) {
		ERaRestart(false);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processActionChip(cJSON* root, uint8_t type) {
	cJSON* dataItem = cJSON_GetObjectItem(root, "data");
	if (!cJSON_IsObject(dataItem)) {
		return;
	}
	cJSON* item = nullptr;
	switch (type) {
		case ERaChipCfgT::CHIP_UPDATE_CONFIG:
			item = cJSON_GetObjectItem(dataItem, "configuration");
			if (cJSON_IsString(item)) {
#if defined(ERA_MODBUS)
				Base::ERaModbus::parseModbusConfig(item->valuestring, false);
				Base::ERaModbus::clearDataBuff();
#endif
			}
			break;
		case ERaChipCfgT::CHIP_UPDATE_CONTROL:
			item = cJSON_GetObjectItem(dataItem, "control");
			if (cJSON_IsString(item)) {
#if defined(ERA_MODBUS)
				Base::ERaModbus::parseModbusConfig(item->valuestring, true);
#endif
			}
			break;
		case ERaChipCfgT::CHIP_CONTROL_ALIAS:
			item = cJSON_GetObjectItem(dataItem, "commands");
			if (cJSON_IsArray(item)) {
				cJSON* keyItem = nullptr;
				for(int i = 0; i < cJSON_GetArraySize(item); ++i) {
					keyItem = cJSON_GetArrayItem(item, i);
					if(cJSON_IsString(keyItem)) {
						// Handle command
#if defined(ERA_MODBUS)
						Base::ERaModbus::addModbusAction(keyItem->valuestring);
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
void ERaProto<Transp, Flash>::processDownCommand(cJSON* root, cJSON* item) {
	if (ERaStrCmp(item->valuestring, "finalize_configuration")) {
		this->processFinalize(root);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processFinalize(cJSON* root) {
	cJSON* item = cJSON_GetObjectItem(root, "configuration");
	if (cJSON_IsObject(item)) {
		this->processConfiguration(item);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processConfiguration(cJSON* root) {
	cJSON* item = cJSON_GetObjectItem(root, "arduino_pin");
	if (cJSON_IsObject(item)) {
		Base::ERaPinRp.deleteAll();
		this->processDeviceConfig(item, ERaChipCfgT::CHIP_IO_PIN);
		this->storePinConfig(item);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processDeviceConfig(cJSON* root, uint8_t type) {
	cJSON* item = cJSON_GetObjectItem(root, "devices");
	if (!cJSON_IsArray(item)) {
		return;
	}
	cJSON* current = nullptr;

	for (current = item->child; current != nullptr; current = current->next) {
		switch (type) {
			case ERaChipCfgT::CHIP_IO_PIN:
				this->processIOPin(current);
				break;
			default:
				break;
		}
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::processIOPin(cJSON* root) {
	if (!cJSON_IsObject(root)) {
		return;
	}
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
		if (ERaStrCmp(current->string, "read_pins")) {
			Base::handleReadPin(current);
		}
		if (ERaStrCmp(current->string, "control_pins")) {
			Base::handleWritePin(current);
		}
		if (ERaStrCmp(current->string, "virtual_pins")) {
			Base::handleVirtualPin(current);
		}
	}
}

#if defined(ERA_ZIGBEE)
	template <class Transp, class Flash>
	void ERaProto<Transp, Flash>::processZigbeeRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
		if (arrayTopic.size() == 3) {
			if ((arrayTopic.at(2) == "permit_to_join") ||
				(arrayTopic.at(2) == "remove_device")) {
				this->processDeviceZigbee(payload);
				return;
			}
		}
		if (arrayTopic.size() < 4) {
			return;
		}
		if (arrayTopic.at(3) == "down") {
			this->processActionZigbee(arrayTopic.at(2), payload, ZigbeeActionT::ZIGBEE_ACTION_SET);
		}
	}

	template <class Transp, class Flash>
	void ERaProto<Transp, Flash>::processDeviceZigbee(const std::string& payload) {
		cJSON* root = cJSON_Parse(payload.c_str());
		if (!cJSON_IsObject(root)) {
			cJSON_Delete(root);
			root = nullptr;
			return;
		}

		cJSON* item = cJSON_GetObjectItem(root, "action");
		if (item == cJSON_Invalid) {
			item = cJSON_GetObjectItem(root, "command");
		}
		if (cJSON_IsString(item)) {
			if (ERaStrCmp(item->valuestring, "search_device")) {
				this->processActionDeviceZigbee(root, ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN);
			}
			else if (ERaStrCmp(item->valuestring, "remove_zigbee")) {
				this->processActionDeviceZigbee(root, ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE);
			}
		}

		cJSON_Delete(root);
		root = nullptr;
		item = nullptr;
	}

	template <class Transp, class Flash>
	void ERaProto<Transp, Flash>::processActionDeviceZigbee(const cJSON* const root, uint8_t type) {
		cJSON* data = cJSON_GetObjectItem(root, "data");
		if (!cJSON_IsObject(root)) {
			return;
		}

		cJSON* item = nullptr;
		cJSON* payload = nullptr;
		switch (type) {
			case ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN:
				item = cJSON_GetObjectItem(data, "zigbee");
				if (cJSON_IsBool(item)) {
					payload = cJSON_CreateObject();
					if (payload == nullptr) {
						break;
					}

					cJSON_AddBoolToObject(payload, "value", item->valueint);
					cJSON* ieeeItem = cJSON_GetObjectItem(data, "ieee_addr");
					if (cJSON_IsString(ieeeItem)) {
						cJSON_AddStringToObject(payload, "ieee_addr", ieeeItem->valuestring);
					}
					cJSON* nwkItem = cJSON_GetObjectItem(data, "nwk_addr");
					if (cJSON_IsString(nwkItem)) {
						cJSON_AddStringToObject(payload, "nwk_addr", nwkItem->valuestring);
					}

					if (!Base::Zigbee::addZigbeeAction(ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN, "", payload)) {
						cJSON_Delete(payload);
					}
					ieeeItem = nullptr;
					nwkItem = nullptr;
				}
				break;
			case ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE:
				item = cJSON_GetObjectItem(data, "ieee_addr");
				if (item == cJSON_Invalid) {
					item = cJSON_GetObjectItem(data, "ieee_address");
				}
				if (cJSON_IsString(item)) {
					this->processActionZigbee(item->valuestring, "{}", ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE);
				}
				break;
			default:
				break;
		}
		item = nullptr;
		data = nullptr;
		payload = nullptr;
	}

	template <class Transp, class Flash>
	void ERaProto<Transp, Flash>::processActionZigbee(const std::string& ieeeAddr, const std::string& payload, uint8_t type) {
		cJSON* root = cJSON_Parse(payload.c_str());
		if (!cJSON_IsObject(root)) {
			cJSON_Delete(root);
			root = nullptr;
			return;
		}

		if (!Base::Zigbee::addZigbeeAction(static_cast<ZigbeeActionT>(type), ieeeAddr.c_str(), root)) {
			cJSON_Delete(root);
		}
		root = nullptr;
	}
#endif

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendInfo() {
	bool status {false};
	cJSON* root = cJSON_CreateObject();
	if(root == nullptr) {
		return false;
	}

	Base::addInfo(root);

    char topic[MAX_TOPIC_LENGTH] {0};
	char* payload = cJSON_PrintUnformatted(root);
    FormatString(topic, this->ERA_TOPIC);
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
bool ERaProto<Transp, Flash>::sendData(ERaRsp_t& rsp) {
	switch (rsp.type) {
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
		case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
		case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
		case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
			return this->sendPinData(rsp);
		case ERaTypeWriteT::ERA_WRITE_CONFIG_ID:
			return this->sendConfigIdData(rsp);
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI:
		case ERaTypeWriteT::ERA_WRITE_CONFIG_ID_MULTI:
			return this->sendConfigIdMultiData(rsp);
		case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA:
			return this->sendModbusData(rsp);
		case ERaTypeWriteT::ERA_WRITE_ZIGBEE_DATA:
			return this->sendZigbeeData(rsp);
		default:
			return false;
	}
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendPinData(ERaRsp_t& rsp) {
	int configId = rsp.id;
	int pMode = Base::ERaPinRp.findPinMode(rsp.id.getInt());
	switch (rsp.type) {
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN: {
				configId = Base::ERaPinRp.findVPinConfigId(rsp.id.getInt());
				if (configId == -1) {
					break;
				}
				rsp.id = configId;
				sendConfigIdData(rsp);
				return true;
			}
			break;
		case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
			if ((pMode == OUTPUT) || (pMode == INPUT) ||
				(pMode == INPUT_PULLUP) || (pMode == INPUT_PULLDOWN)) {
				configId = Base::ERaPinRp.findConfigId(rsp.id.getInt());
				if (configId == -1) {
					break;
				}
				rsp.id = configId;
				sendConfigIdData(rsp);
				return true;
			}
			break;
		case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
			if (pMode == ANALOG) {
				configId = Base::ERaPinRp.findConfigId(rsp.id.getInt());
				if (configId == -1) {
					break;
				}
				rsp.id = configId;
				sendConfigIdData(rsp);
				return true;
			}
			break;
		case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
			if (pMode == PWM) {
				configId = Base::ERaPinRp.findConfigId(rsp.id.getInt());
				if (configId == -1) {
					break;
				}
				rsp.id = configId;
				sendConfigIdData(rsp);
				return true;
			}
			break;
		default:
			break;
	}

	char name[50] {0};
	bool status {false};
	char* payload = nullptr;
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->ERA_TOPIC);
	FormatString(topicName, "/pin/data");
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return false;
	}
	switch (rsp.type) {
	case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
		FormatString(name, "virtual_pin_%d", rsp.id.getInt());
		break;
	case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
		FormatString(name, "digital_pin_%d", rsp.id.getInt());
		break;
	case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
		FormatString(name, "analog_pin_%d", rsp.id.getInt());
		break;
	case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
		FormatString(name, "pwm_pin_%d", rsp.id.getInt());
		break;
	default:
		cJSON_Delete(root);
		root = nullptr;
		return false;
	}
	if (rsp.param.getString() != nullptr) {
		cJSON_AddStringToObject(root, name, rsp.param);
	}
	else {
		cJSON_AddNumberWithDecimalToObject(root, name, rsp.param, 5);
	}
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
bool ERaProto<Transp, Flash>::sendConfigIdData(ERaRsp_t& rsp) {
	bool status {false};
	char* payload = nullptr;
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->ERA_TOPIC);
	FormatString(topicName, "/config/%d/value", rsp.id.getInt());
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return false;
	}
	if (rsp.param.getString() != nullptr) {
		cJSON_AddStringToObject(root, "v", rsp.param);
	}
	else {
		cJSON_AddNumberWithDecimalToObject(root, "v", rsp.param, 5);
	}
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
bool ERaProto<Transp, Flash>::sendConfigIdMultiData(ERaRsp_t& rsp) {
	bool status {false};
	char* payload = rsp.param;
	if (payload == nullptr) {
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->ERA_TOPIC);
	FormatString(topicName, "/config_value");
	status = this->transp.publishData(topicName, payload);
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendModbusData(ERaRsp_t& rsp) {
	bool status {false};
	char* payload = rsp.param;
	if (payload == nullptr) {
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->ERA_TOPIC);
	FormatString(topicName, "/data");
	status = this->transp.publishData(topicName, payload);
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
bool ERaProto<Transp, Flash>::sendZigbeeData(ERaRsp_t& rsp) {
	bool status {false};
	char* topic = rsp.id;
	char* payload = rsp.param;
	if (topic == nullptr) {
		return false;
	}
	if (payload == nullptr) {
		topic = nullptr;
		return false;
	}
	char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, this->ERA_TOPIC);
	FormatString(topicName, topic);
	status = this->transp.publishData(topicName, payload);
	topic = nullptr;
	payload = nullptr;
	return status;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommand(const char* auth, ERaRsp_t& rsp, ApiData_t data) {
	if (auth == nullptr) {
		return;
	}

	char* payload = nullptr;
	char topicName[MAX_TOPIC_LENGTH] {0};
    FormatString(topicName, "%s/%s", BASE_TOPIC, auth);
	switch (rsp.type) {
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN:
			FormatString(topicName, "/virtual_pin/%d", rsp.id.getInt());
			break;
		case ERaTypeWriteT::ERA_WRITE_DIGITAL_PIN:
		case ERaTypeWriteT::ERA_WRITE_ANALOG_PIN:
		case ERaTypeWriteT::ERA_WRITE_PWM_PIN:
			FormatString(topicName, "/arduino_pin/%d", rsp.id.getInt());
			break;
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI: {
			ERaDataJson* vrData = (ERaDataJson*)data;
			this->sendCommandMulti(auth, rsp, vrData);
		}
			return;
		default:
			return;
	}

	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return;
	}
	if (rsp.param.getString() != nullptr) {
		cJSON_AddStringToObject(root, "value", rsp.param);
	}
	else {
		cJSON_AddNumberWithDecimalToObject(root, "value", rsp.param, 5);
	}
	payload = cJSON_PrintUnformatted(root);
	if (payload != nullptr) {
		this->transp.publishData(topicName, payload);
	}
	cJSON_Delete(root);
	free(payload);
	root = nullptr;
	payload = nullptr;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommandMulti(const char* auth, ERaRsp_t& rsp, ERaDataJson* data) {
	if (data == nullptr) {
		return;
	}
	rsp.type = ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN;
	const ERaDataJson::iterator e = data->end();
	for (ERaDataJson::iterator it = data->begin(); it != e; ++it) {
		if (it.getName() == nullptr) {
			continue;
		}
		rsp.id = atoi(it.getName());
		if (it.isString()) {
			rsp.param = it.getString();
		}
		else {
			rsp.param = it.getDouble();
		}
		this->sendCommand(auth, rsp);
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommand(ERaRsp_t& rsp, ApiData_t data) {
	switch (rsp.type) {
		case ERaTypeWriteT::ERA_WRITE_VIRTUAL_PIN_MULTI: {
			ERaDataJson* vrData = (ERaDataJson*)data;
			this->sendCommandVirtual(rsp, vrData);
		}
			break;
		case ERaTypeWriteT::ERA_WRITE_MODBUS_DATA: {
			ERaDataBuff* modbusData = (ERaDataBuff*)data;
			this->sendCommandModbus(rsp, modbusData);
		}
			break;
		default:
			break;
	}
	this->sendData(rsp);
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommandVirtual(ERaRsp_t& rsp, ERaDataJson* data) {
	if (data == nullptr) {
		return;
	}
	ERaDataJson::iterator current;
	const ERaDataJson::iterator e = data->end();
	for (ERaDataJson::iterator it = data->begin(); it != e;) {
		if (it.getName() == nullptr) {
			continue;
		}
		int configId = Base::ERaPinRp.findVPinConfigId(atoi(it.getName()));
		if (configId == -1) {
			if (it.isString()) {
				Base::virtualWrite(atoi(it.getName()), it.getString());
			}
			else {
				Base::virtualWrite(atoi(it.getName()), it.getDouble());
			}
			data->remove(it);
			if (!current) {
				it = data->begin();
			}
			else {
				it = current;
				++it;
			}
			continue;
		}
		char name[2 + 8 * sizeof(configId)] {0};
		FormatString(name, "%d", configId);
		it.rename(name);
		current = it;
		++it;
	}
	if (current) {
		rsp.param = data->getObject();
	}
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::sendCommandModbus(ERaRsp_t& rsp, ERaDataBuff* data) {
	if (data == nullptr) {
		return;
	}
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return;
	}
	int mbTotal {0};
	int mbFailed {0};
	int mbWriteFailed {0};
	size_t index {0};
	size_t dataLen = data->getLen() + 1;
	char* mbData = (char*)ERA_CALLOC(dataLen, sizeof(char));
	if (mbData == nullptr) {
		return;
	}
	char* mbAck = (char*)ERA_CALLOC(dataLen, sizeof(char));
	if (mbAck == nullptr) {
		return;
	}
	const ERaDataBuff::iterator e = data->end();
	for (ERaDataBuff::iterator it = data->begin(); it < e; ++it) {
		if (it == "fail_read") {
			++it;
			mbFailed = it.getInt();
		}
		else if (it == "fail_write") {
			++it;
			mbWriteFailed = it.getInt();
		}
		else if (it == "total") {
			++it;
			mbTotal = it.getInt();
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
	cJSON_AddNumberToObject(root, "modbus_write_fail", mbWriteFailed);
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
void ERaProto<Transp, Flash>::initPinConfig() {
    char* ptr = nullptr;
    ptr = Base::readFromFlash(FILENAME_CONFIG);
    this->parsePinConfig(ptr);
    free(ptr);
	ptr = nullptr;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::parsePinConfig(const char* str) {
	if (str == nullptr) {
		return;
	}
	cJSON* root = cJSON_Parse(str);
	if (cJSON_IsObject(root)) {
		this->processDeviceConfig(root, ERaChipCfgT::CHIP_IO_PIN);
	}
	cJSON_Delete(root);
	root = nullptr;
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::storePinConfig(const cJSON* const root) {
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
void ERaProto<Transp, Flash>::removePinConfig() {
	Base::ERaPinRp.deleteAll();
	Base::removeFromFlash(FILENAME_CONFIG);
}

template <class Transp, class Flash>
unsigned int ERaProto<Transp, Flash>::splitString(const std::string& strInput, std::vector<std::string>& retList, char ch) {
	if (strInput.empty()) {
		return -1;
	}

	unsigned int pos = strInput.find(ch);
	unsigned int size = strInput.size();
	unsigned int initPos = 0;
	retList.clear();

	while (pos != std::string::npos) {
		std::string str_sub = strInput.substr(initPos, pos - initPos);// initPos+1);
		retList.push_back(str_sub);
		initPos = pos + 1;
		pos = strInput.find(ch, initPos);
	}

	std::string lastSub = strInput.substr(initPos, std::min(pos, size) - initPos + 1);
	retList.push_back(lastSub);
	return retList.size();
}

#endif /* INC_ERA_PROTOCOL_HPP_ */
