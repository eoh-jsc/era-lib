#ifndef INC_ERA_SIMPLE_SERIAL_HPP_
#define INC_ERA_SIMPLE_SERIAL_HPP_

#include <Adapters/ERaSerialClient.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashArduino.hpp>

#define ERA_MODEL_TYPE                "ERa"
#define ERA_BOARD_TYPE                "Stream"

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::runRead();
	Base::Modbus::runWrite();
#endif
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, static_cast<Proto*>(this)->ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, "Stream");
    cJSON_AddStringToObject(root, INFO_BSSID, "Stream");
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, "Stream");
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, "Stream");
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "Stream");
}

static ERaMqtt<ERaClient, MQTTClient> mqtt;
static ERaFlash flash;
ERaStream ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_SERIAL_HPP_ */
