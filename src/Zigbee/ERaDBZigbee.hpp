#ifndef INC_ERA_DATABASE_ZIGBEE_HPP_
#define INC_ERA_DATABASE_ZIGBEE_HPP_

#include <stdint.h>
#include <Utility/ERacJSON.hpp>
#include "definition/ERaDefineZigbee.hpp"

template <class Zigbee>
class ERaDBZigbee
{
    const char* TAG = "DBZigbee";
    const char* FILENAME_CONFIG = FILENAME_ZIGBEE_CONFIG;
    const char* FILENAME_DEVICES = FILENAME_ZIGBEE_DEVICES;

public:
    ERaDBZigbee()
        : device(InfoDevice_t::instance())
        , coordinator(InfoCoordinator_t::instance())
        , needFinalize(false)
        , mutex(NULL)
    {
        memset(this->hashID, 0, sizeof(this->hashID));
    }
    ~ERaDBZigbee()
    {}

protected:
    bool isNeedFinalize() const;
    bool updateHashID(const char* hash, bool check);

    void parseZigbeeConfig();
    void parseZigbeeConfig(const char* str);
    void storeZigbeeConfig(const char* str);

    void parseConfigMapZigbee(const cJSON* const root);
    void parseConfigMapDevices(const cJSON* const root);

    void processConfigMapZigbee(cJSON* root, const char* hash, const char* buf);

    void parseZigbeeDevice();
    void storeZigbeeDevice();
    IdentDeviceAddr_t* getDeviceFromCoordinator();

    template <int size>
    IdentDeviceAddr_t* getDeviceFromCoordinator(const uint8_t(&ieee)[size]);

    IdentDeviceAddr_t* setDeviceToCoordinator(bool final);

    void lock() {
        ERaGuardLock(this->mutex);
    }

    void unlock() {
        ERaGuardUnlock(this->mutex);
    }

    bool isNewReport(const IdentDeviceAddr_t* deviceInfo) const {
        return deviceInfo->configReport;
    }

    bool isChangedReport(IdentDeviceAddr_t* deviceInfo) const {
        bool ret = (deviceInfo->configReport != deviceInfo->prevConfigReport);
        deviceInfo->prevConfigReport = deviceInfo->configReport;
        return ret;
    }

    void setConfigReport(IdentDeviceAddr_t* deviceInfo, bool enable) const {
        deviceInfo->configReport = enable;
    }

private:
    void parseConfigMapDevice(const cJSON* const root);
    void parseConfigMapKey(IdentDeviceAddr_t* deviceInfo, const cJSON* const root);

    void parseDevice(const cJSON* const root);
    cJSON* createDevice(const IdentDeviceAddr_t& deviceInfo);
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

    mutable bool needFinalize;
    char hashID[37];
    ERaMutex_t mutex;
};

template <class Zigbee>
bool ERaDBZigbee<Zigbee>::isNeedFinalize() const {
    if (this->coordinator == nullptr) {
        return false;
    }
    else if (this->coordinator->lock) {
        return false;
    }
    bool ret = this->needFinalize;
    this->needFinalize = false;
    return ret;
}

template <class Zigbee>
bool ERaDBZigbee<Zigbee>::updateHashID(const char* hash, bool check) {
    if (!check) {
    }
    else if (this->coordinator == nullptr) {
        return false;
    }
    else if (this->coordinator->lock) {
        this->needFinalize = true;
        return false;
    }
    else {
        this->needFinalize = false;
    }

    if (hash == nullptr) {
        return false;
    }
    if (strcmp(this->hashID, hash)) {
        snprintf(this->hashID, sizeof(this->hashID), hash);
        return true;
    }
    return false;
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseConfigMapZigbee(const cJSON* const root) {
    cJSON* zigbee = cJSON_GetObjectItem(root, "zigbee");
    if (!cJSON_IsObject(zigbee)) {
        return;
    }

    this->parseConfigMapDevices(zigbee);
    this->coordinator->loadPrevConfigReport();
    ERA_LOG(TAG, ERA_PSTR("Zigbee configuration loaded from flash"));
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseConfigMapDevices(const cJSON* const root) {
    ERaGuardAuto(this->mutex);

    cJSON* devices = cJSON_GetObjectItem(root, "devices");
    if (!cJSON_IsArray(devices)) {
        return;
    }

    if (this->coordinator == nullptr) {
        return;
    }
    if (!this->coordinator->deviceCount) {
        return;
    }
    this->coordinator->clearConfigMap();

    size_t size = cJSON_GetArraySize(devices);
    for (size_t i = 0; i < size; ++i) {
        cJSON* device = cJSON_GetArrayItem(devices, i);
        if (!cJSON_IsObject(device)) {
            continue;
        }
        this->parseConfigMapDevice(device);
    }

    this->coordinator->loadConfigReport();
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseConfigMapDevice(const cJSON* const root) {
    cJSON* ieee = cJSON_GetObjectItem(root, "ieee_address");
    if (!cJSON_IsString(ieee)) {
        return;
    }

    uint8_t ieeeAddr[LENGTH_EXTADDR_IEEE] {0};
    if (!StringToIEEE(ieee->valuestring, ieeeAddr)) {
        return;
    }
    if (IsZeroArray(ieeeAddr)) {
        return;
    }

    IdentDeviceAddr_t* deviceInfo = this->getDeviceFromCoordinator(ieeeAddr);
    if (deviceInfo == nullptr) {
        return;
    }

    cJSON* keys = cJSON_GetObjectItem(root, "keys");
    if (!cJSON_IsArray(keys)) {
        return;
    }

    size_t size = cJSON_GetArraySize(keys);
    for (size_t i = 0; i < size; ++i) {
        cJSON* key = cJSON_GetArrayItem(keys, i);
        if (!cJSON_IsObject(key)) {
            continue;
        }
        this->parseConfigMapKey(deviceInfo, key);
    }
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseConfigMapKey(IdentDeviceAddr_t* deviceInfo, const cJSON* const root) {
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
        if (deviceInfo->numConfigMap >= MAX_CONFIG_MAP_ZIGBEE) {
            return;
        }

        ConfigIdMap_t& config = deviceInfo->configMap[deviceInfo->numConfigMap++];
        ClearMem(config.key);
        CopyString(current->string, config.key);
        config.configId = current->valueint;
    }
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseZigbeeConfig() {
    // Read from flash
    char* ptr = nullptr;
    ptr = this->thisZigbee().readDataFromFlash(FILENAME_CONFIG);
    this->parseZigbeeConfig(ptr);
    free(ptr);
    ptr = nullptr;
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseZigbeeConfig(const char* str) {
    cJSON* root = cJSON_Parse(str);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    cJSON* item = cJSON_GetObjectItem(root, "hash_id");
    if (cJSON_IsString(item)) {
        this->updateHashID(item->valuestring, false);
    }
    item = cJSON_GetObjectItem(root, "configuration");
    if (cJSON_IsObject(item)) {
        this->parseConfigMapZigbee(item);
    }

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::processConfigMapZigbee(cJSON* root, const char* hash, const char* buf) {
    if (!this->updateHashID(hash, true)) {
        return;
    }

    this->parseConfigMapDevices(root);
    this->storeZigbeeConfig(buf);
    ERaWriteConfig(ERaConfigTypeT::ERA_ZIGBEE_CONFIG);
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::storeZigbeeConfig(const char* str) {
    if (str == nullptr) {
        return;
    }

    this->thisZigbee().writeDataToFlash(FILENAME_CONFIG, str);
    ERA_LOG(TAG, ERA_PSTR("Zigbee configuration stored to flash"));
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::parseDevice(const cJSON* const root) {
    if (this->coordinator->deviceCount >= MAX_DEVICE_ZIGBEE) {
        return;
    }

    IdentDeviceAddr_t& deviceInfo = this->coordinator->deviceIdent[this->coordinator->deviceCount++];

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
    cJSON* item = nullptr;
    // Begin read from flash
    this->thisZigbee().beginReadFromFlash(FILENAME_DEVICES);
    do {
        // Read line from flash
        ptr = this->thisZigbee().readLineFromFlash();
        if (ptr == nullptr) {
            break;
        }
        item = cJSON_Parse(ptr);
        if (cJSON_IsObject(item)) {
            this->parseDevice(item);
        }
        free(ptr);
        cJSON_Delete(item);
    } while (ptr != nullptr);

    // End read from flash
    this->thisZigbee().endReadFromFlash();
    this->checkDevice();
    this->parseZigbeeConfig();
    item = nullptr;
    ptr = nullptr;
}

template <class Zigbee>
cJSON* ERaDBZigbee<Zigbee>::createDevice(const IdentDeviceAddr_t& deviceInfo) {
    cJSON* item = cJSON_CreateObject();
    if (item == nullptr) {
        return nullptr;
    }

    cJSON_AddNumberToObject(item, "type", deviceInfo.typeDevice);
    cJSON_AddNumberToObject(item, "nwk_addr", deviceInfo.address.addr.nwkAddr);
    cJSON_AddStringToObject(item, "ieee_addr", IEEEToString(deviceInfo.address.addr.ieeeAddr).c_str());
    if (deviceInfo.appVer) {
        cJSON_AddNumberToObject(item, "app_ver", deviceInfo.appVer);
    }
    cJSON_AddStringToObject(item, "model", deviceInfo.modelName);

    return item;
}

template <class Zigbee>
void ERaDBZigbee<Zigbee>::storeZigbeeDevice() {
    if (this->coordinator->lock) {
        return;
    }

    char* ptr = nullptr;
    cJSON* item = nullptr;
    // Begin write to flash
    this->thisZigbee().beginWriteToFlash(FILENAME_DEVICES);
    for (size_t i = 0; i < this->coordinator->deviceCount; ++i) {
        item = this->createDevice(this->coordinator->deviceIdent[i]);
        ptr = cJSON_PrintUnformatted(item);
        cJSON_Delete(item);
        if (ptr != nullptr) {
            // Write line to flash
            this->thisZigbee().writeLineToFlash(ptr);
            free(ptr);
        }
    }
    // End write to flash
    this->thisZigbee().endWriteToFlash();
    item = nullptr;
    ptr = nullptr;
}

template <class Zigbee>
IdentDeviceAddr_t* ERaDBZigbee<Zigbee>::getDeviceFromCoordinator() {
    return this->getDeviceFromCoordinator(this->device->address.addr.ieeeAddr);
}

template <class Zigbee>
template <int size>
IdentDeviceAddr_t* ERaDBZigbee<Zigbee>::getDeviceFromCoordinator(const uint8_t(&ieee)[size]) {
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                    find_deviceWithIEEEAddr_t(ieee));
    return ((deviceInfo != std::end(this->coordinator->deviceIdent)) ? deviceInfo : nullptr);
}

template <class Zigbee>
IdentDeviceAddr_t* ERaDBZigbee<Zigbee>::setDeviceToCoordinator(bool final) {
    if (IsZeroArray(this->device->address.addr.ieeeAddr)) {
        return nullptr;
    }
    IdentDeviceAddr_t* deviceInfo = this->getDeviceFromCoordinator();
    if (deviceInfo != nullptr) {
        deviceInfo->address.addr.nwkAddr = this->device->address.addr.nwkAddr;
        if (!final) {
            return deviceInfo;
        }
    }
    else {
        if ((this->coordinator->deviceCount >= MAX_DEVICE_ZIGBEE) ||
            !strlen(this->device->modelName)) {
            return nullptr;
        }
        deviceInfo = &this->coordinator->deviceIdent[this->coordinator->deviceCount++];
        deviceInfo->address.addr.nwkAddr = this->device->address.addr.nwkAddr;
        CopyArray(this->device->address.addr.ieeeAddr, deviceInfo->address.addr.ieeeAddr);
    }
    deviceInfo->typeDevice = this->device->typeDevice;
    deviceInfo->appVer = this->device->appVer;
    if (strlen(this->device->modelName)) {
        ClearMem(deviceInfo->modelName);
        CopyString(this->device->modelName, deviceInfo->modelName);
    }
    if (final) {
        this->storeZigbeeDevice();
    }
    return deviceInfo;
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
