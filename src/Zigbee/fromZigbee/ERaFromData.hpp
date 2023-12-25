#ifndef INC_ERA_FROM_DATA_ZIGBEE_HPP_
#define INC_ERA_FROM_DATA_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
IdentDeviceAddr_t* ERaFromZigbee<Zigbee>::createDataGlobal(const DataAFMsg_t& afMsg, uint16_t attribute, uint8_t type, uint64_t& value) {
    bool defined {false};
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                                find_deviceWithNwkAddr_t(afMsg.srcAddr.addr.nwkAddr));
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_INIT_MAX)) {
            return nullptr;
        }
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_JOINED) ||
            ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING)) {
            if (afMsg.srcAddr.addr.nwkAddr == this->device->address.addr.nwkAddr) {
                deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                            find_deviceWithIEEEAddr_t(this->device->address.addr.ieeeAddr));
            }
            if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
                return nullptr;
            }
        }
    }
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        if (afMsg.zclId == ClusterIDT::ZCL_CLUSTER_GREEN_POWER) {
            return nullptr;
        }
        if (this->thisZigbee().Zigbee::ToZigbee::CommandZigbee::requestIEEEAddrZstack(const_cast<DataAFMsg_t&>(afMsg).srcAddr, 0, 0) != ResultT::RESULT_SUCCESSFUL) {
            return nullptr;
        }
        deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                            find_deviceWithNwkAddr_t(afMsg.srcAddr.addr.nwkAddr));
        if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
            return nullptr;
        }
    }

    if (deviceInfo->data.topic == nullptr) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_INIT_MAX)) {
            return nullptr;
        }
        if (IsZeroArray(deviceInfo->address.addr.ieeeAddr)) {
            return nullptr;
        }
        deviceInfo->data.topic = reinterpret_cast<char*>(ERA_CALLOC(MAX_TOPIC_LENGTH, sizeof(char)));
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, "/zigbee/");
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, IEEEToString(deviceInfo->address.addr.ieeeAddr).c_str());
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, "/data");
        deviceInfo->receiveId = afMsg.receiveId + 1;
    }

    deviceInfo->isConnected = true;
    const_cast<DataAFMsg_t&>(afMsg).deviceInfo = deviceInfo;

    bool isSameId {false};
    cJSON* dataItem = nullptr;

    if (deviceInfo->data.payload == nullptr) {
        deviceInfo->data.payload = cJSON_CreateObject();
        if (deviceInfo->data.payload == nullptr) {
            return nullptr;
        }
    }

    isSameId = (deviceInfo->receiveId == afMsg.receiveId);
    deviceInfo->receiveId = afMsg.receiveId;

    cJSON_SetStringToObject(deviceInfo->data.payload, "type", "zigbee_data");

    dataItem = cJSON_GetObjectItem(deviceInfo->data.payload, "data");
    if (!cJSON_IsObject(dataItem)) {
        cJSON_DeleteItemFromObject(deviceInfo->data.payload, "data");
        dataItem = cJSON_CreateObject();
        if (dataItem == nullptr) {
            return nullptr;
        }
        cJSON_AddItemToObject(deviceInfo->data.payload, "data", dataItem);
    }

    cJSON_SetStringToObject(dataItem, "model", deviceInfo->modelName);
    cJSON_SetNumberToObject(dataItem, "nwk_addr", deviceInfo->address.addr.nwkAddr);
    cJSON_SetNumberToObject(dataItem, "parent", afMsg.parentAddr);
    cJSON_SetNumberToObject(dataItem, "radius", afMsg.radius);
    cJSON_SetNumberToObject(dataItem, "lqi", afMsg.linkQuality);
    cJSON_SetNumberToObject(dataItem, "ddr", afMsg.ddr);

    switch (afMsg.zclId) {
        case ClusterIDT::ZCL_CLUSTER_POWER_CONFIG:
            defined = this->powerConfigFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_ONOFF:
            defined = this->onOffFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL:
            defined = this->levelFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC:
            defined = this->multistateInputFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE:
            defined = this->temperatureMeasFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE:
            defined = this->pressureMeasFromZigbee(afMsg, dataItem, attribute, value);
            break;
        case ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY:
            defined = this->humidityMeasFromZigbee(afMsg, dataItem, attribute, value);
            break;
        default:
            break;
    }

    ERA_FORCE_UNUSED(defined);

    dataItem = nullptr;
    if (isSameId) {
        return nullptr;
    }
    return deviceInfo;
}

template <class Zigbee>
IdentDeviceAddr_t* ERaFromZigbee<Zigbee>::createDataSpecific(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp) {
    bool defined {false};
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                                find_deviceWithNwkAddr_t(afMsg.srcAddr.addr.nwkAddr));
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_INIT_MAX)) {
            return nullptr;
        }
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_JOINED) ||
            ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING)) {
            if (afMsg.srcAddr.addr.nwkAddr == this->device->address.addr.nwkAddr) {
                deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                            find_deviceWithIEEEAddr_t(this->device->address.addr.ieeeAddr));
            }
            if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
                return nullptr;
            }
        }
    }
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        if (afMsg.zclId == ClusterIDT::ZCL_CLUSTER_GREEN_POWER) {
            return nullptr;
        }
        if (this->thisZigbee().Zigbee::ToZigbee::CommandZigbee::requestIEEEAddrZstack(const_cast<DataAFMsg_t&>(afMsg).srcAddr, 0, 0) != ResultT::RESULT_SUCCESSFUL) {
            return nullptr;
        }
        deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                            find_deviceWithNwkAddr_t(afMsg.srcAddr.addr.nwkAddr));
        if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
            return nullptr;
        }
    }

    if (deviceInfo->data.topic == nullptr) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_INIT_MAX)) {
            return nullptr;
        }
        if (IsZeroArray(deviceInfo->address.addr.ieeeAddr)) {
            return nullptr;
        }
        deviceInfo->data.topic = reinterpret_cast<char*>(ERA_CALLOC(MAX_TOPIC_LENGTH, sizeof(char)));
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, "/zigbee/");
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, IEEEToString(deviceInfo->address.addr.ieeeAddr).c_str());
        FormatString(deviceInfo->data.topic, MAX_TOPIC_LENGTH, "/data");
        deviceInfo->receiveId = afMsg.receiveId + 1;
    }

    deviceInfo->isConnected = true;
    const_cast<DataAFMsg_t&>(afMsg).deviceInfo = deviceInfo;

    bool isSameId {false};
    cJSON* dataItem = nullptr;

    if (deviceInfo->data.payload == nullptr) {
        deviceInfo->data.payload = cJSON_CreateObject();
        if (deviceInfo->data.payload == nullptr) {
            return nullptr;
        }
    }

    isSameId = (deviceInfo->receiveId == afMsg.receiveId);
    deviceInfo->receiveId = afMsg.receiveId;

    cJSON_SetStringToObject(deviceInfo->data.payload, "type", "zigbee_data");

    dataItem = cJSON_GetObjectItem(deviceInfo->data.payload, "data");
    if (!cJSON_IsObject(dataItem)) {
        cJSON_DeleteItemFromObject(deviceInfo->data.payload, "data");
        dataItem = cJSON_CreateObject();
        if (dataItem == nullptr) {
            return nullptr;
        }
        cJSON_AddItemToObject(deviceInfo->data.payload, "data", dataItem);
    }

    cJSON_SetStringToObject(dataItem, "model", deviceInfo->modelName);
    cJSON_SetNumberToObject(dataItem, "nwk_addr", deviceInfo->address.addr.nwkAddr);
    cJSON_SetNumberToObject(dataItem, "parent", afMsg.parentAddr);
    cJSON_SetNumberToObject(dataItem, "radius", afMsg.radius);
    cJSON_SetNumberToObject(dataItem, "lqi", afMsg.linkQuality);
    cJSON_SetNumberToObject(dataItem, "ddr", afMsg.ddr);

    switch (afMsg.zclId) {
        case ClusterIDT::ZCL_CLUSTER_ONOFF:
            defined = this->onOffSpecificFromZigbee(afMsg, dataItem, "", defaultRsp);
            break;
        case ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL:
            defined = this->levelSpecificFromZigbee(afMsg, dataItem, "", defaultRsp);
            break;
        default:
            break;
    }

    ERA_FORCE_UNUSED(defined);

    dataItem = nullptr;
    if (isSameId) {
        return nullptr;
    }
    this->thisZigbee().publishZigbeeData(deviceInfo);
    return deviceInfo;
}

template <class Zigbee>
template <typename T>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, T value) {
    char name[LENGTH_BUFFER] {0};
#if defined(ENABLE_SCALE_ZIGBEE_DATA)
    const ScaleDataZigbee_t* element = std::find_if(std::begin(ScaleZigbeeList), std::end(ScaleZigbeeList), [key](const ScaleDataZigbee_t& e) {
        return ((e.key != nullptr) && (key != nullptr) && CompareString(e.key, key));
    });
    if (element != std::end(ScaleZigbeeList)) {
        float scale {element->defaultScale};
        if (element->numScale) {
            const ScaleDataModel_t* elementScale = std::find_if(std::begin(element->scaleModel), std::end(element->scaleModel), [afMsg](const ScaleDataModel_t& e) {
                if ((e.modelName == nullptr) || (afMsg.deviceInfo == nullptr)) {
                    return false;
                }
                const char* pModel = strstr(e.modelName, "*");
                if (pModel != nullptr) {
                    return CompareNString(e.modelName, afMsg.deviceInfo->modelName, strlen(e.modelName) - strlen(pModel));
                }
                else {
                    return CompareString(e.modelName, afMsg.deviceInfo->modelName);
                }
            });
            if (elementScale != std::end(element->scaleModel)) {
                scale = elementScale->scale;
            }
        }
        if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
            cJSON_SetNumberWithDecimalToObject(root, key, value * scale, 5);
        }
        else {
            StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
            cJSON_SetNumberWithDecimalToObject(root, name, value * scale, 5);
        }
        return;
    }
#endif
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_SetNumberToObject(root, key, value);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_SetNumberToObject(root, name, value);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, float value) {
    char name[LENGTH_BUFFER] {0};
    float scale {1.0f};
#if defined(ENABLE_SCALE_ZIGBEE_DATA)
    const ScaleDataZigbee_t* element = std::find_if(std::begin(ScaleZigbeeList), std::end(ScaleZigbeeList), [key](const ScaleDataZigbee_t& e) {
        return ((e.key != nullptr) && (key != nullptr) && CompareString(e.key, key));
    });
    if (element != std::end(ScaleZigbeeList)) {
        scale = element->defaultScale;
        if (element->numScale) {
            const ScaleDataModel_t* elementScale = std::find_if(std::begin(element->scaleModel), std::end(element->scaleModel), [afMsg](const ScaleDataModel_t& e) {
                if ((e.modelName == nullptr) || (afMsg.deviceInfo == nullptr)) {
                    return false;
                }
                const char* pModel = strstr(e.modelName, "*");
                if (pModel != nullptr) {
                    return CompareNString(e.modelName, afMsg.deviceInfo->modelName, strlen(e.modelName) - strlen(pModel));
                }
                else {
                    return CompareString(e.modelName, afMsg.deviceInfo->modelName);
                }
            });
            if (elementScale != std::end(element->scaleModel)) {
                scale = elementScale->scale;
            }
        }
    }
#endif
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_SetNumberWithDecimalToObject(root, key, value * scale, 5);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_SetNumberWithDecimalToObject(root, name, value * scale, 5);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, double value) {
    char name[LENGTH_BUFFER] {0};
    float scale {1.0f};
#if defined(ENABLE_SCALE_ZIGBEE_DATA)
    const ScaleDataZigbee_t* element = std::find_if(std::begin(ScaleZigbeeList), std::end(ScaleZigbeeList), [key](const ScaleDataZigbee_t& e) {
        return ((e.key != nullptr) && (key != nullptr) && CompareString(e.key, key));
    });
    if (element != std::end(ScaleZigbeeList)) {
        scale = element->defaultScale;
        if (element->numScale) {
            const ScaleDataModel_t* elementScale = std::find_if(std::begin(element->scaleModel), std::end(element->scaleModel), [afMsg](const ScaleDataModel_t& e) {
                if ((e.modelName == nullptr) || (afMsg.deviceInfo == nullptr)) {
                    return false;
                }
                const char* pModel = strstr(e.modelName, "*");
                if (pModel != nullptr) {
                    return CompareNString(e.modelName, afMsg.deviceInfo->modelName, strlen(e.modelName) - strlen(pModel));
                }
                else {
                    return CompareString(e.modelName, afMsg.deviceInfo->modelName);
                }
            });
            if (elementScale != std::end(element->scaleModel)) {
                scale = elementScale->scale;
            }
        }
    }
#endif
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_SetNumberWithDecimalToObject(root, key, value * scale, 5);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_SetNumberWithDecimalToObject(root, name, value * scale, 5);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, char* value) {
    char name[LENGTH_BUFFER] {0};
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_SetStringToObject(root, key, value);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_SetStringToObject(root, name, value);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, const char* value) {
    char name[LENGTH_BUFFER] {0};
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_SetStringToObject(root, key, value);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_SetStringToObject(root, name, value);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, cJSON* value) {
    char name[LENGTH_BUFFER] {0};
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_AddItemToObject(root, key, value);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_AddItemToObject(root, name, value);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, const cJSON* value) {
    char name[LENGTH_BUFFER] {0};
    if (afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        cJSON_AddItemToObject(root, key, value);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        cJSON_AddItemToObject(root, name, value);
    }
}

template <class Zigbee>
cJSON* ERaFromZigbee<Zigbee>::getDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key) {
    char name[LENGTH_BUFFER] {0};
    cJSON* item = nullptr;
    if(afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        item = cJSON_GetObjectItem(root, key);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        item = cJSON_GetObjectItem(root, name);
    }
    return item;
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::removeDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key) {
    char name[LENGTH_BUFFER] {0};
    cJSON* item = nullptr;
    if(afMsg.srcAddr.endpoint == EndpointListT::ENDPOINT1) {
        item = cJSON_DetachItemFromObject(root, key);
    }
    else {
        StringPrint(name, "%s_%d", key, afMsg.srcAddr.endpoint);
        item = cJSON_DetachItemFromObject(root, name);
    }
    if (item != nullptr) {
        cJSON_Delete(item);
        item = nullptr;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::createDeviceEvent(const DeviceEventT event, const AFAddrType_t* dstAddr) {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON* dataItem = cJSON_CreateObject();
    if (dataItem == nullptr) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }
    cJSON* definitionItem = cJSON_CreateObject();
    if (definitionItem == nullptr)
    {
        cJSON_Delete(dataItem);
        cJSON_Delete(root);
        dataItem = nullptr;
        root = nullptr;
        return;
    }

    cJSON_AddNumberToObject(dataItem, "nwk_addr", ((dstAddr != nullptr) ? dstAddr->addr.nwkAddr : this->device->address.addr.nwkAddr));
    cJSON_AddStringToObject(dataItem, "ieee_addr", IEEEToString((dstAddr != nullptr) ? dstAddr->addr.ieeeAddr : this->device->address.addr.ieeeAddr).c_str());

    if (event != DeviceEventT::DEVICE_EVENT_ANNOUNCE) {
        cJSON_AddStringToObject(definitionItem, "model", this->device->modelName);
        cJSON_AddStringToObject(definitionItem, "vendor", this->device->manufName);
        cJSON_AddNumberToObject(definitionItem, "vendor_code", this->device->manufCode);
    }

    switch (event) {
        case DeviceEventT::DEVICE_EVENT_JOINED:
            cJSON_AddStringToObject(root, "type", "device_joined");
            cJSON_Delete(definitionItem);
            break;
        case DeviceEventT::DEVICE_EVENT_ANNOUNCE:
            cJSON_AddStringToObject(root, "type", "device_announce");
            cJSON_AddBoolToObject(definitionItem, "alternate_pan_id", this->device->annceDevice.alternatePanId);
            switch (this->device->annceDevice.type) {
                case TypeAnnceDeviceT::ANNCE_ENDDEVICE:
                    cJSON_AddStringToObject(definitionItem, "device_type", "End Device");
                    break;
                case TypeAnnceDeviceT::ANNCE_ROUTERDEVICE:
                    cJSON_AddStringToObject(definitionItem, "device_type", "Router");
                    break;
                default:
                    cJSON_AddNumberToObject(definitionItem, "device_type", this->device->annceDevice.type);
                    break;
            }
            switch (this->device->annceDevice.power) {
                case PowerSourceAnnceT::PWS_ANNCE_MAIN_POWER:
                    cJSON_AddStringToObject(definitionItem, "power", "main power");
                    break;
                default:
                    cJSON_AddStringToObject(definitionItem, "power", "other");
                    break;
            }
            cJSON_AddBoolToObject(definitionItem, "rx_when_idle", this->device->annceDevice.isIdle);
            cJSON_AddItemToObject(dataItem, "definition", definitionItem);
            break;
        case DeviceEventT::DEVICE_EVENT_INTERVIEW_STARTED:
        case DeviceEventT::DEVICE_EVENT_INTERVIEW_BASIC_INFO:
            cJSON_AddStringToObject(root, "type", "device_interview");
            cJSON_AddStringToObject(dataItem, "status", "started");
            cJSON_AddItemToObject(dataItem, "definition", definitionItem);
            break;
        case DeviceEventT::DEVICE_EVENT_INTERVIEW_SUCCESSFUL:
            cJSON_AddStringToObject(root, "type", "device_interview");
            cJSON_AddStringToObject(dataItem, "status", "successful");
            switch (this->device->typeDevice) {
                case TypeDeviceT::COORDINATOR:
                    cJSON_AddStringToObject(definitionItem, "device_type", "Coordinator");
                    break;
                case TypeDeviceT::ROUTERDEVICE:
                    cJSON_AddStringToObject(definitionItem, "device_type", "Router");
                    break;
                case TypeDeviceT::ENDDEVICE:
                    cJSON_AddStringToObject(definitionItem, "device_type", "End Device");
                    break;
                default:
                    cJSON_AddNumberToObject(definitionItem, "device_type", this->device->typeDevice);
                    break;
            }
            switch (this->device->power) {
                case PowerSourceT::PWS_UNKNOWN0:
                    cJSON_AddStringToObject(definitionItem, "power", "unknown");
                    break;
                case PowerSourceT::PWS_SINGLE_PHASE:
                    cJSON_AddStringToObject(definitionItem, "power", "single phase");
                    break;
                case PowerSourceT::PWS_THREE_PHASE:
                    cJSON_AddStringToObject(definitionItem, "power", "three phase");
                    break;
                case PowerSourceT::PWS_BATTERY:
                    cJSON_AddStringToObject(definitionItem, "power", "battery");
                    if(this->device->batVoltage) {
                        cJSON_AddNumberToObject(definitionItem, "battery_voltage", this->device->batVoltage);
                    }
                    if(this->device->batPercent) {
                        cJSON_AddNumberToObject(definitionItem, "battery_percent", this->device->batPercent);
                    }
                    break;
                case PowerSourceT::PWS_DC_SOURCE:
                    cJSON_AddStringToObject(definitionItem, "power", "DC source");
                    break;
                case PowerSourceT::PWS_EMERGENCY_MAIN:
                    cJSON_AddStringToObject(definitionItem, "power", "emergency main");
                    break;
                case PowerSourceT::PWS_EMERGENCY_MAIN_SW:
                    cJSON_AddStringToObject(definitionItem, "power", "emergency main switch");
                    break;
                default:
                    cJSON_AddNumberToObject(definitionItem, "power", this->device->power);
                    break;
            }
            cJSON_AddNumberToObject(definitionItem, "app_ver", this->device->appVer);
            cJSON_AddNumberToObject(definitionItem, "zcl_ver", this->device->zclVer);
            cJSON_AddNumberToObject(definitionItem, "stack_ver", this->device->stackVer);
            cJSON_AddNumberToObject(definitionItem, "hw_ver", this->device->hwVer);
            cJSON_AddStringToObject(definitionItem, "data_code", this->device->dataCode);
            cJSON_AddStringToObject(definitionItem, "sw_build", this->device->swBuild);
            cJSON_AddNumberToObject(definitionItem, "parent", this->device->parentAddr);
            cJSON_AddBoolToObject(definitionItem, "ias", this->device->ias);
            if(this->device->pollControl) {
                cJSON_AddItemToObject(definitionItem, "poll_control", this->createDevicePollControl());
            }
            else {
                cJSON_AddBoolToObject(definitionItem, "poll_control", false);
            }
            cJSON_AddItemToObject(definitionItem, "endpoints", this->createDeviceEndpoints());
            cJSON_AddItemToObject(dataItem, "definition", definitionItem);
            break;
        case DeviceEventT::DEVICE_EVENT_INTERVIEW_FAILED:
            cJSON_AddStringToObject(root, "type", "device_interview");
            cJSON_AddStringToObject(dataItem, "status", "failed");
            cJSON_Delete(definitionItem);
            break;
        case DeviceEventT::DEVICE_EVENT_LEAVE:
            cJSON_AddStringToObject(root, "type", "device_leave");
            cJSON_Delete(definitionItem);
            break;
        default:
            break;
    }

    cJSON_AddItemToObject(root, "data", dataItem);

    this->thisZigbee().publishZigbeeData(TOPIC_ZIGBEE_BRIDGE_EVENT, root, true, false);
    if ((event == DeviceEventT::DEVICE_EVENT_INTERVIEW_SUCCESSFUL) ||
        (event == DeviceEventT::DEVICE_EVENT_INTERVIEW_BASIC_INFO)) {
        this->thisZigbee().publishZigbeeData(TOPIC_ZIGBEE_DEVICE_EVENT, root, true, false);
    }

    cJSON_Delete(root);
    root = nullptr;
    dataItem = nullptr;
    definitionItem = nullptr;
}

template <class Zigbee>
cJSON* ERaFromZigbee<Zigbee>::createDeviceEndpoints() {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return nullptr;
    }

    for (size_t i = 0; i < this->device->epCount; ++i) {
        char name[10] {0};
        FormatString(name, "%d", this->device->epList[i].endpoint);
        cJSON* epItem = cJSON_CreateObject();
        if (epItem == nullptr) {
            continue;
        }
        cJSON* clusterItem = cJSON_CreateObject();
        if (clusterItem == nullptr) {
            cJSON_Delete(epItem);
            continue;
        }

        cJSON* zclItem = cJSON_CreateObject();
        if (zclItem == nullptr) {
            cJSON_Delete(epItem);
            cJSON_Delete(clusterItem);
            continue;
        }
        cJSON* zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].inZclCount; ++j) {
                cJSON* numItem = cJSON_CreateNumber(this->device->epList[i].inZclIdList[j]);
                if (numItem != nullptr) {
                    cJSON_AddItemToArray(zclArr, numItem);
                }
            }
            cJSON_AddItemToObject(zclItem, "zcl_id", zclArr);
        }
        zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].inZclCount; ++j) {
                const StringifyZcl_t* zclName = std::find_if(std::begin(StrifyZcl), std::end(StrifyZcl), [=](const StringifyZcl_t& e) {
                    return e.zclId == this->device->epList[i].inZclIdList[j];
                });
                if (zclName != std::end(StrifyZcl)) {
                    cJSON* strItem = cJSON_CreateString(zclName->zclName);
                    if (strItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, strItem);
                    }
                }
                else {
                    cJSON* numItem = cJSON_CreateNumber(this->device->epList[i].inZclIdList[j]);
                    if (numItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, numItem);
                    }
                }
            }
            cJSON_AddItemToObject(zclItem, "zcl_name", zclArr);
        }
        zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].inZclCount; ++j) {
                const KeyDataZigbee_t* key = std::find_if(std::begin(KeyDataZb), std::end(KeyDataZb), [=](const KeyDataZigbee_t& e) {
                    return e.zclId == this->device->epList[i].inZclIdList[j];
                });
                if (key == std::end(KeyDataZb)) {
                    continue;
                }
                for (size_t k = 0; k < key->numKeyIn; ++k) {
                    if (key->keyInZcl[k] == nullptr) {
                        continue;
                    }
                    char keyName[LENGTH_BUFFER] {0};
                    if (this->device->epList[i].endpoint > EndpointListT::ENDPOINT1) {
                        FormatString(keyName, "%s_%d", key->keyInZcl[k], this->device->epList[i].endpoint);
                    }
                    else {
                        FormatString(keyName, key->keyInZcl[k]);
                    }
                    cJSON* strItem = cJSON_CreateString(keyName);
                    if (strItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, strItem);
                    }
                }
            }
            cJSON_AddItemToObject(zclItem, "keys", zclArr);
        }
        cJSON_AddItemToObject(clusterItem, "input", zclItem);

        zclItem = cJSON_CreateObject();
        if (zclItem == nullptr) {
            cJSON_Delete(epItem);
            cJSON_Delete(clusterItem);
            continue;
        }
        zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].outZclCount; ++j) {
                cJSON* numItem = cJSON_CreateNumber(this->device->epList[i].outZclIdList[j]);
                if (numItem != nullptr) {
                    cJSON_AddItemToArray(zclArr, numItem);
                }
            }
            cJSON_AddItemToObject(zclItem, "zcl_id", zclArr);
        }
        zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].outZclCount; ++j) {
                const StringifyZcl_t* zclName = std::find_if(std::begin(StrifyZcl), std::end(StrifyZcl), [=](const StringifyZcl_t& e) {
                    return e.zclId == this->device->epList[i].outZclIdList[j];
                });
                if (zclName != std::end(StrifyZcl)) {
                    cJSON* strItem = cJSON_CreateString(zclName->zclName);
                    if (strItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, strItem);
                    }
                }
                else {
                    cJSON* numItem = cJSON_CreateNumber(this->device->epList[i].outZclIdList[j]);
                    if (numItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, numItem);
                    }
                }
            }
            cJSON_AddItemToObject(zclItem, "zcl_name", zclArr);
        }
        zclArr = cJSON_CreateArray();
        if (zclArr != nullptr) {
            for (size_t j = 0; j < this->device->epList[i].outZclCount; ++j) {
                const KeyDataZigbee_t* key = std::find_if(std::begin(KeyDataZb), std::end(KeyDataZb), [=](const KeyDataZigbee_t& e) {
                    return e.zclId == this->device->epList[i].outZclIdList[j];
                });
                if (key == std::end(KeyDataZb)) {
                    continue;
                }
                for (size_t k = 0; k < key->numKeyOut; ++k) {
                    if (key->keyOutZcl[k] == nullptr) {
                        continue;
                    }
                    char keyName[LENGTH_BUFFER] {0};
                    if (this->device->epList[i].endpoint > EndpointListT::ENDPOINT1) {
                        FormatString(keyName, "%s_%d", key->keyOutZcl[k], this->device->epList[i].endpoint);
                    }
                    else {
                        FormatString(keyName, key->keyOutZcl[k]);
                    }
                    cJSON* strItem = cJSON_CreateString(keyName);
                    if (strItem != nullptr) {
                        cJSON_AddItemToArray(zclArr, strItem);
                    }
                }
            }
            cJSON_AddItemToObject(zclItem, "keys", zclArr);
        }
        cJSON_AddItemToObject(clusterItem, "output", zclItem);

        cJSON_AddItemToObject(epItem, "clusters", clusterItem);
        cJSON_AddItemToObject(root, name, epItem);
    }

    return root;
}

template <class Zigbee>
cJSON* ERaFromZigbee<Zigbee>::createDevicePollControl() {
    if (!this->device->pollControl) {
        return nullptr;
    }

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return nullptr;
    }

    if (this->device->checkinInterval && this->device->checkinInterval <= 2400) {
        cJSON_AddStringToObject(root, "type", "fast poll");
    }
    else {
        cJSON_AddStringToObject(root, "type", "active");
    }
    cJSON_AddNumberToObject(root, "checkin_interval", this->device->checkinInterval);

    return root;
}

#endif /* INC_ERA_FROM_DATA_ZIGBEE_HPP_ */
