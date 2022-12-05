#ifndef INC_ERA_DATABASE_ZIGBEE_HPP_
#define INC_ERA_DATABASE_ZIGBEE_HPP_

#include <stdint.h>
#include <Utility/ERacJSON.hpp>
#include "definition/ERaDefineZigbee.hpp"

template <class Zigbee>
class ERaDBZigbee
{
    const char* FILENAME_DEVICES = FILENAME_ZIGBEE_DEVICES;

public:
    ERaDBZigbee()
        : device(InfoDevice_t::instance)
        , coordinator(InfoCoordinator_t::instance)
    {}
    ~ERaDBZigbee()
    {}

protected:
    void parseZigbeeDevice();
    void storeZigbeeDevice();
    IdentDeviceAddr_t* getDeviceFromCoordinator();
    void setDeviceToCoordinator();

private:
    void parseDevice(const cJSON* const root);
    void createDevice(cJSON* const root, const IdentDeviceAddr_t& deviceInfo);
    void checkDevice();

	inline
	const Zigbee& thisZigbee() const {
		return static_cast<const Zigbee&>(*this);
	}

	inline
	Zigbee& thisZigbee() {
		return static_cast<Zigbee&>(*this);
	}

    InfoDevice_t*& device;
    InfoCoordinator_t*& coordinator;
};

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseDevice(const cJSON* const root) {
    if (this->coordinator->deviceCount >= MAX_DEVICE_ZIGBEE) {
        return;
    }

    IdentDeviceAddr_t& deviceInfo = coordinator->deviceIdent[coordinator->deviceCount++];

    deviceInfo.isConnected = true;
    cJSON* typeItem = cJSON_GetObjectItem(root, "type");
	if (cJSON_IsNumber(typeItem)) {
		deviceInfo.typeDevice = typeItem->valueint;
    }
    cJSON* nwkItem = cJSON_GetObjectItem(root, "nwk_addr");
	if (cJSON_IsNumber(nwkItem)) {
		deviceInfo.address.addr.nwkAddr = nwkItem->valueint;
    }
    cJSON* ieeeItem = cJSON_GetObjectItem(root, "ieee_addr");
	if (cJSON_IsString(ieeeItem)) {
		StringToIEEE(ieeeItem->valuestring, deviceInfo.address.addr.ieeeAddr);
    }
	cJSON* appVerItem = cJSON_GetObjectItem(root, "app_ver");
	if (cJSON_IsNumber(appVerItem)) {
		deviceInfo.appVer = appVerItem->valueint;
    }
	cJSON* modelItem = cJSON_GetObjectItem(root, "model");
	if (cJSON_IsString(modelItem)) {
		ClearMem(deviceInfo.modelName);
        CopyString(modelItem->valuestring, deviceInfo.modelName);
    }
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseZigbeeDevice() {
    char* ptr = nullptr;
    ptr = this->thisZigbee().readDataFromFlash(FILENAME_DEVICES);
    if (ptr == nullptr) {
        return;
    }

    cJSON* root = cJSON_Parse(ptr);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "devices");
    if (cJSON_IsArray(item)) {
        int sizeItem = cJSON_GetArraySize(item);
        for (int i = 0; i < sizeItem; ++i) {
            cJSON* subItem = cJSON_GetArrayItem(item, i);
            if (cJSON_IsObject(subItem)) {
                this->parseDevice(subItem);
            }
        }
    }

    this->checkDevice();
    cJSON_Delete(root);
    free(ptr);
    root = nullptr;
	ptr = nullptr;
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::createDevice(cJSON* const root, const IdentDeviceAddr_t& deviceInfo) {
    cJSON* item = cJSON_CreateObject();
    if (item == nullptr) {
        return;
    }

    cJSON_AddNumberToObject(item, "type", deviceInfo.typeDevice);
    cJSON_AddNumberToObject(item, "nwk_addr", deviceInfo.address.addr.nwkAddr);
    cJSON_AddStringToObject(item, "ieee_addr", IEEEToString(deviceInfo.address.addr.ieeeAddr).c_str());
	if (deviceInfo.appVer) {
		cJSON_AddNumberToObject(item, "app_ver", deviceInfo.appVer);
    }
	cJSON_AddStringToObject(item, "model", deviceInfo.modelName);

    cJSON_AddItemToArray(root, item);
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::storeZigbeeDevice() {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }

    cJSON* devicesItem = cJSON_CreateArray();
    if (devicesItem == nullptr) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    for (size_t i = 0; i < this->coordinator->deviceCount; ++i) {
        this->createDevice(devicesItem, this->coordinator->deviceIdent[i]);
    }
    cJSON_AddItemToObject(root, "devices", devicesItem);

	char* ptr = cJSON_PrintUnformatted(root);
    if (ptr != nullptr) {
        //store
        this->thisZigbee().writeDataToFlash(FILENAME_DEVICES, ptr);
    }
    cJSON_Delete(root);
    free(ptr);
    root = nullptr;
    devicesItem = nullptr;
    ptr = nullptr;
}

template <class Zigbee>
IdentDeviceAddr_t* ERaDBZigbee<Zigbee>::getDeviceFromCoordinator() {
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                    find_deviceWithIEEEAddr_t(this->device->address.addr.ieeeAddr));
    return ((deviceInfo != std::end(this->coordinator->deviceIdent)) ? deviceInfo : nullptr);
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::setDeviceToCoordinator() {
    if (IsZeroArray(this->device->address.addr.ieeeAddr)) {
        return;
    }
    IdentDeviceAddr_t* deviceInfo = this->getDeviceFromCoordinator();
    if (deviceInfo != nullptr) {
        if (deviceInfo->address.addr.nwkAddr == this->device->address.addr.nwkAddr) {
            return;
        }
        deviceInfo->address.addr.nwkAddr = this->device->address.addr.nwkAddr;
    }
    else {
        if (this->coordinator->deviceCount >= MAX_DEVICE_ZIGBEE ||
            !strlen(this->device->modelName)) {
            return;
        }
        deviceInfo = &this->coordinator->deviceIdent[this->coordinator->deviceCount++];
        deviceInfo->address.addr.nwkAddr = this->device->address.addr.nwkAddr;
        CopyArray(this->device->address.addr.ieeeAddr, deviceInfo->address.addr.ieeeAddr);
    }
    deviceInfo->typeDevice = this->device->typeDevice;
    deviceInfo->appVer = this->device->appVer;
    ClearMem(deviceInfo->modelName);
    CopyString(this->device->modelName, deviceInfo->modelName);
    this->storeZigbeeDevice();
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::checkDevice() {
    if (!this->coordinator->deviceCount) {
        return;
    }
    for (int i = this->coordinator->deviceCount - 1; i >= 0; --i) {
        if (IsZeroArray(this->coordinator->deviceIdent[i].address.addr.ieeeAddr) ||
            !strlen(this->coordinator->deviceIdent[i].modelName)) {
			std::move(std::begin(this->coordinator->deviceIdent) + i + 1, std::end(this->coordinator->deviceIdent), std::begin(this->coordinator->deviceIdent) + i);
			this->coordinator->deviceCount--;
        }
    }
}

#endif /* INC_ERA_DATABASE_ZIGBEE_HPP_ */
