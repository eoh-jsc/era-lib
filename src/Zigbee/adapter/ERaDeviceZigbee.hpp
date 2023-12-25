#ifndef INC_ERA_DEVICE_ZIGBEE_HPP_
#define INC_ERA_DEVICE_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

template <class Api>
bool ERaZigbee<Api>::interviewDevice() {
    IdentDeviceAddr_t* deviceInfo = DBZigbee::getDeviceFromCoordinator();
    if ((deviceInfo != nullptr) &&
        (deviceInfo->address.addr.nwkAddr == this->device->address.addr.nwkAddr)) {
        return false;
    }

    EndpointListT dstEndPoint {EndpointListT::ENDPOINT1};

    ERA_LOG(TAG, ERA_PSTR("Interview network address %s(%d) started"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                                    this->device->address.addr.nwkAddr);

    FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_STARTED);

    ToZigbee::CommandZigbee::requestNodeDesc(this->device->address, 1); /* Inquire about the Node Descriptor information of the dest device */
    ToZigbee::CommandZigbee::extRouterDiscovery(this->device->address, this->Options, this->Radius); /* ZDO route extension message */

    for (size_t i = 0; i < 5; ++i) {
        if (this->device->hasManufCode) {
            break;
        }
        if (ToZigbee::CommandZigbee::requestNodeDesc(this->device->address, 1) != ResultT::RESULT_SUCCESSFUL) { /* Inquire about the Node Descriptor information of the dest device */
            ToZigbee::CommandZigbee::extRouterDiscovery(this->device->address, this->Options, this->Radius); /* ZDO route extension message */
        }
    }

    if (!this->device->hasManufCode) {
        ERA_LOG(TAG, ERA_PSTR("Interview network address %s(%d) failed"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                                        this->device->address.addr.nwkAddr);
        FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_FAILED);
        return false;
    }

    if ((this->device->manufCode == 4619) && 
        (this->device->typeDevice == TypeDeviceT::ENDDEVICE)) {
        ERaDelay(5000);
    }

    if (ToZigbee::CommandZigbee::requestListEndpoint(this->device->address, this->device->address.addr.nwkAddr, 1) != ResultT::RESULT_SUCCESSFUL) { /* Request a list of active endpoint from the dest device */
        ERA_LOG(TAG, ERA_PSTR("Interview network address %s(%d) failed"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                                        this->device->address.addr.nwkAddr);
        FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_FAILED);
        return false;
    }

    for (size_t i = 0; i < this->device->epCount; ++i) {
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT_NONE) {
            continue;
        }
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT242) {
            continue;
        }
        this->device->address.endpoint = this->device->epList[i].endpoint;
        ToZigbee::CommandZigbee::requestSimpleDesc(this->device->address, 10); /* Descriptor of the dest device's Endpoint */
        if (this->device->epList[i].isBasic) {
            dstEndPoint = this->device->epList[i].endpoint;
            if (this->device->pollControl && !this->device->checkin) { /* Handle check-in from sleeping end devices */
                this->device->checkin = true;
                /* check-in poll */
            }

            this->readAttrDevice(this->device->address, ClusterIDT::ZCL_CLUSTER_BASIC, {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MFR_NAME, /* Request a Manufacturer Name from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MODEL_NAME}); /* Request a Model Name from the dest device */

            FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_BASIC_INFO);
            deviceInfo = DBZigbee::setDeviceToCoordinator(false);
            if (deviceInfo == nullptr) {
                ERA_LOG(TAG, ERA_PSTR("Interview network address %s(%d) failed"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                                                this->device->address.addr.nwkAddr);
                FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_FAILED);
                return false;
            }
            this->publishZigbeeData(deviceInfo);

            this->readAttrDevice(this->device->address, ClusterIDT::ZCL_CLUSTER_BASIC, {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_POWER_SOURCE, /* Request a Power Source from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ZCL_VERSION, /* Request a Zcl Version from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_APP_VERSION}); /* Request a App Version from the dest device */

            this->readAttrDevice(this->device->address, ClusterIDT::ZCL_CLUSTER_BASIC, {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_STACK_VERSION, /* Request a Stack Version from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_HARDWARE_VERSION, /* Request a Hardware Version from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_DATE_CODE, /* Request a DateCode from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_SW_BUILD_ID, /* Request a Software Build from the dest device */
                                                                                        ZbZclBasicSvrAttrT::ZCL_ATTR_CLUSTER_REVISION}); /* Request a Revision from the dest device */

            this->device->basicEp = this->device->epList[i].endpoint;
            break;
        }
    }

    for (size_t i = 0; i < this->device->epCount; ++i) {
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT_NONE) {
            continue;
        }
        if (this->device->epList[i].endpoint == dstEndPoint) {
            continue;
        }
        this->device->address.endpoint = this->device->epList[i].endpoint;
        ToZigbee::CommandZigbee::requestSimpleDesc(this->device->address, 10);
    }

    ERA_LOG(TAG, ERA_PSTR("Binding %s(%d)"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                        this->device->address.addr.nwkAddr);

    for (size_t i = 0; i < this->device->epCount; ++i) {
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT_NONE) {
            continue;
        }
        this->device->address.endpoint = this->device->epList[i].endpoint;
        if (this->device->power == PowerSourceT::PWS_BATTERY || this->device->power == PowerSourceT::PWS_DC_SOURCE) {
            if (this->isClusterExist(this->device->epList[i].inZclIdList, this->device->epList[i].outZclIdList, ClusterIDT::ZCL_CLUSTER_POWER_CONFIG)) {
                this->readAttrDevice(this->device->address, ClusterIDT::ZCL_CLUSTER_POWER_CONFIG, {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE,
                                                                                                    ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT});
            }
        }

        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT242) {
            continue;
        }
        vector<ClusterIDT> zclIds;
        for (size_t j = 0; j < this->device->epList[i].inZclCount; ++j) {
            if (this->device->epList[i].inZclIdList[j] == ClusterIDT::ZCL_CLUSTER_BASIC) { /* Skip Bind Basic Cluster */
                continue;
            }
            if ((this->isBindReportExist(ReportingList, this->device->epList[i].inZclIdList[j]) != nullptr) ||
                !this->isElementExist(AllZclId, this->device->epList[i].inZclIdList[j])) {
                zclIds.push_back(this->device->epList[i].inZclIdList[j]);
                ToZigbee::CommandZigbee::requestBind(this->device->address, this->coordinator->address, this->device->epList[i].inZclIdList[j]);
            }
        }
        for (size_t j = 0; j < this->device->epList[i].outZclCount; ++j) {
            if (this->device->epList[i].outZclIdList[j] == ClusterIDT::ZCL_CLUSTER_BASIC) { /* Skip Bind Basic Cluster */
                continue;
            }
            if (this->isElementExist(zclIds, this->device->epList[i].outZclIdList[j])) {
                continue;
            }
            if ((this->isBindReportExist(ReportingList, this->device->epList[i].outZclIdList[j]) != nullptr) ||
                !this->isElementExist(AllZclId, this->device->epList[i].outZclIdList[j])) {
                ToZigbee::CommandZigbee::requestBind(this->device->address, this->coordinator->address, this->device->epList[i].outZclIdList[j]);
            }
        }
    }

    /* Config Reporting */

    ERA_LOG(TAG, ERA_PSTR("Interview network address %s(%d) successful"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                                        this->device->address.addr.nwkAddr);
    FromZigbee::createDeviceEvent(DeviceEventT::DEVICE_EVENT_INTERVIEW_SUCCESSFUL);
    deviceInfo = DBZigbee::setDeviceToCoordinator(true);
    this->publishZigbeeData(deviceInfo);
    return true;
}

template <class Api>
void ERaZigbee<Api>::removeDevice(const cJSON* const root, AFAddrType_t& dstAddr) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return;
    }
    uint8_t option {0};
    cJSON* item = cJSON_GetObjectItem(root, "rejoin");
    if (cJSON_IsBool(item)) {
        option |= (item->valueint & 0x01);
    }
    item = cJSON_GetObjectItem(root, "remove_children");
    if (cJSON_IsBool(item)) {
        option |= ((item->valueint << 1) & 0x02);
    }
    this->removeDeviceWithAddr(dstAddr);
}

template <class Api>
void ERaZigbee<Api>::removeDeviceWithAddr(AFAddrType_t& dstAddr) {
    if (!dstAddr.addr.nwkAddr && IsZeroArray(dstAddr.addr.ieeeAddr)) {
        return;
    }
    bool hasRemove {false};
    IdentDeviceAddr_t* element = std::begin(this->coordinator->deviceIdent);
    while (element != std::end(this->coordinator->deviceIdent)) {
        if (!IsZeroArray(dstAddr.addr.ieeeAddr)) {
            element = std::find_if(element, std::end(this->coordinator->deviceIdent), find_deviceWithIEEEAddr_t(dstAddr.addr.ieeeAddr));
        }
        else {
            element = std::find_if(element, std::end(this->coordinator->deviceIdent), find_deviceWithNwkAddr_t(dstAddr.addr.nwkAddr));
        }
        if (element == std::end(this->coordinator->deviceIdent)) {
            break;
        }
        hasRemove = true;
        if (element->data.topic != nullptr) {
            free(element->data.topic);
            element->data.topic = nullptr;
        }
        if (element->data.payload != nullptr) {
            free(element->data.payload);
            element->data.payload = nullptr;
        }
    }
    if (hasRemove) {
        DBZigbee::storeZigbeeDevice();
    }
}

template <class Api>
ResultT ERaZigbee<Api>::readAttrDevice(AFAddrType_t& dstAddr,
                                        ClusterIDT zclId,
                                        vector<uint16_t> attributes,
                                        size_t limit,
                                        void* value) {
    this->device->readDone = false;
    for (size_t i = 0; i < limit; ++i) {
        if (ToZigbee::CommandZigbee::readAttributes(dstAddr, EndpointListT::ENDPOINT1, zclId,
                                                    ManufacturerCodesT::MANUF_CODE_NONE, attributes,
                                                    AFCommandsT::AF_INCOMING_MSG, value) == ResultT::RESULT_SUCCESSFUL) {
            if (this->device->readDone) {
                return ResultT::RESULT_SUCCESSFUL;
            }
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class Api>
ResultT ERaZigbee<Api>::writeAttrDevice(AFAddrType_t& dstAddr,
                                        ClusterIDT zclId,
                                        vector<DataWriteAttr_t> data,
                                        size_t limit) {
    for (size_t i = 0; i < limit; ++i) {
        if (ToZigbee::CommandZigbee::writeAttributes(dstAddr, EndpointListT::ENDPOINT1, zclId,
                                                    ManufacturerCodesT::MANUF_CODE_NONE, data,
                                                    AFCommandsT::AF_INCOMING_MSG) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class Api>
void ERaZigbee<Api>::readDataDevice() {
    /* Read data device */
    ERA_LOG(TAG, ERA_PSTR("Reading data %s(%d)"), IEEEToString(this->device->address.addr.ieeeAddr).c_str(),
                                                            this->device->address.addr.nwkAddr);

    if (this->device->typeDevice != TypeDeviceT::ROUTERDEVICE) {
        if ((this->device->power != PowerSourceT::PWS_SINGLE_PHASE) &&
            (this->device->power != PowerSourceT::PWS_THREE_PHASE)) {
            return;
        }
    }
    for (size_t i = 0; i < this->device->epCount; ++i) {
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT_NONE) {
            continue;
        }
        if (this->device->epList[i].endpoint == EndpointListT::ENDPOINT242) {
            continue;
        }
        this->device->address.endpoint = this->device->epList[i].endpoint;
        vector<ClusterIDT> zclIds;
        const ConfigBindReport_t* config = nullptr;
        for (size_t j = 0; j < this->device->epList[i].inZclCount; ++j) {
            if ((config = this->isBindReportExist(ReportingList, this->device->epList[i].inZclIdList[j])) != nullptr) {
                zclIds.push_back(this->device->epList[i].inZclIdList[j]);
                this->readAttrDevice(this->device->address, this->device->epList[i].inZclIdList[j], {
                    config->attribute, config->attribute + config->numReadAttr
                    }, 1);
            }
        }
        for (size_t j = 0; j < this->device->epList[i].outZclCount; ++j) {
            if (this->isElementExist(zclIds, this->device->epList[i].outZclIdList[j])) {
                continue;
            }
            if ((config = this->isBindReportExist(ReportingList, this->device->epList[i].outZclIdList[j])) != nullptr) {
                this->readAttrDevice(this->device->address, this->device->epList[i].outZclIdList[j], {
                    config->attribute, config->attribute + config->numReadAttr
                    }, 1);
            }
        }
    }
}

template <class Api>
template <int inSize, int outSize>
bool ERaZigbee<Api>::isClusterExist(const ClusterIDT(&inZclList)[inSize], const ClusterIDT(&outZclList)[outSize], const ClusterIDT zclId) {
    const ClusterIDT* inZcl = std::find_if(std::begin(inZclList), std::end(inZclList), [zclId](const ClusterIDT& e) {
        return e == zclId;
    });
    const ClusterIDT* outZcl = std::find_if(std::begin(outZclList), std::end(outZclList), [zclId](const ClusterIDT& e) {
        return e == zclId;
    });
    return ((inZcl != std::end(inZclList)) || (outZcl != std::end(outZclList)));
}

template <class Api>
template <int size>
const ConfigBindReport_t* ERaZigbee<Api>::isBindReportExist(const ConfigBindReport_t(&inConfig)[size], const ClusterIDT zclId) {
    const ConfigBindReport_t* config = std::find_if(std::begin(inConfig), std::end(inConfig), [zclId](const ConfigBindReport_t& e) {
        return e.zclId == zclId;
    });
    return ((config != std::end(inConfig)) ? config : nullptr);
}

template <class Api>
template <typename T, int size>
bool ERaZigbee<Api>::isElementExist(const T(&elementList)[size], const T element) {
    const T* _element = std::find_if(std::begin(elementList), std::end(elementList), [element](const T& e) {
        return e == element;
    });
    return (_element != std::end(elementList));
}

template <class Api>
template <typename T>
bool ERaZigbee<Api>::isElementExist(const std::vector<T>& elementList, const T element) {
    const auto _element = std::find_if(elementList.begin(), elementList.end(), [element](const T& e) {
        return e == element;
    });
    return (_element != elementList.end());
}

#endif /* INC_ERA_DEVICE_ZIGBEE_HPP_ */
