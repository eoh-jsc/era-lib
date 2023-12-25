#ifndef INC_ERA_FROM_GLOBAL_ZIGBEE_HPP_
#define INC_ERA_FROM_GLOBAL_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processFrameTypeGlobal(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {
    defaultRsp.cmdId = afMsg.cmdId;
    switch (afMsg.cmdId) {
        case CommandIdT::READ_ATTR:
            this->processReadAttribute(afMsg, defaultRsp, value);
            break;
        case CommandIdT::READ_ATTR_RSP:
            this->processReadAttributeResponse(afMsg, defaultRsp, value);
            break;
        case CommandIdT::REPORT_ATTR:
            this->processReportAttribute(afMsg, defaultRsp, value);
            break;
        case CommandIdT::DEFAULT_RSP:
            this->processDefaultResponse(afMsg, defaultRsp, value);
            break;
        default:
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReadAttribute(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {

}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReadAttributeNormal(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp) {
    this->device->valueAttr.zclId = afMsg.zclId;
    this->device->valueAttr.attribute = BUILD_UINT16(afMsg.pData[0]);
    ZCLStatusT status = static_cast<ZCLStatusT>(afMsg.pData[2]);
    if (status == ZCLStatusT::SUCCESS_ZCL) {
        this->device->valueAttr.type = static_cast<DataTypeT>(afMsg.pData[3]);
    }
    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {});
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReadAttributeResponse(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {
    if (afMsg.pDataLen < ZCL_DATA_MIN) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING)) {
            this->device->readDone = true;
        }
        return;
    }
    uint16_t attribute = BUILD_UINT16(afMsg.pData[0]);
    ZCLStatusT status = static_cast<ZCLStatusT>(afMsg.pData[2]);
    if (!ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING)) {
        this->processReadAttributeNormal(afMsg, defaultRsp);
    }
    else {
        if (afMsg.srcAddr.addr.nwkAddr != this->device->address.addr.nwkAddr) {
            return;
        }
        this->device->readDone = true;
    }
    switch (afMsg.zclId) {
        case ClusterIDT::ZCL_CLUSTER_BASIC: {
            switch (attribute) {
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MFR_NAME:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MODEL_NAME:
                    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MFR_NAME, this->device->manufName},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_MODEL_NAME, this->device->modelName}
                    });
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_POWER_SOURCE:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ZCL_VERSION:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_APP_VERSION:
                    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_POWER_SOURCE, &this->device->power},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ZCL_VERSION, &this->device->zclVer},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_APP_VERSION, &this->device->appVer}
                    });
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_STACK_VERSION:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_HARDWARE_VERSION:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_DATE_CODE:
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_SW_BUILD_ID:
                case ZbZclBasicSvrAttrT::ZCL_ATTR_CLUSTER_REVISION:
                    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_STACK_VERSION, &this->device->stackVer},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_HARDWARE_VERSION, &this->device->hwVer},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_DATE_CODE, this->device->dataCode},
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_SW_BUILD_ID, this->device->swBuild},
                        {ZbZclBasicSvrAttrT::ZCL_ATTR_CLUSTER_REVISION, &this->device->revision}
                    });
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_LOCATION:
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ENVIRONMENT:
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ENABLED:
                    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {
                        {ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ENABLED, &this->device->enableDevice}
                    });
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_ALARM_MASK:
                    break;
                case ZbZclBasicSvrAttrT::ZCL_BASIC_ATTR_DISABLE_LOCAL_CONFIG:
                    break;
                default:
                    break;
            }
        }
            break;
        case ClusterIDT::ZCL_CLUSTER_POWER_CONFIG: {
            switch (attribute) {
                case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE:
                case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT:
                    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {
                        {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE, &this->device->batVoltage},
                        {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT, &this->device->batPercent}
                    });
                    break;
                default:
                    break;
            }
        }
            break;
        default:
            break;
    }

    ERA_FORCE_UNUSED(status);
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReportAttribute(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {
    if (afMsg.pDataLen < ZCL_DATA_MIN) {
        return;
    }

    uint16_t attribute = BUILD_UINT16(afMsg.pData[0]);
    DataTypeT dataType = static_cast<DataTypeT>(afMsg.pData[2]);

    switch (afMsg.zclId) {
        case ZCL_CLUSTER_BASIC:
            return;
        default:
            break;
    }
    this->getDataAttributes(afMsg, defaultRsp, afMsg.srcAddr.addr.nwkAddr, afMsg.srcAddr.endpoint, {});

    ERA_FORCE_UNUSED(attribute);
    ERA_FORCE_UNUSED(dataType);
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processDefaultResponse(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {
    defaultRsp.hasResponse = true;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::getDataAttributes(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, uint16_t nwkAddr, EndpointListT endpoint, vector<DataAttr_t> listAttr) {
    if (afMsg.srcAddr.addrMode != AddressModeT::ADDR_16BIT) {
        return false;
    }
    if (nwkAddr != afMsg.srcAddr.addr.nwkAddr) {
        return false;
    }
    if (endpoint != afMsg.srcAddr.endpoint) {
        return false;
    }
    if (!afMsg.pDataLen || afMsg.pData == nullptr) {
        return false;
    }
    
    bool result {false};
    IdentDeviceAddr_t* deviceInfo {nullptr};
    uint8_t mLen = ((afMsg.cmdId == CommandIdT::READ_ATTR_RSP) ? 1 : 0);
    uint8_t shiftPos {0};
    uint8_t status {0};
    uint8_t position {0};
    uint8_t len {0};
    uint8_t type {0};
    uint16_t attrRsp {0};
    uint8_t numElement {0};
    uint64_t value {0};

    for (int i = 0; i < afMsg.pDataLen; i = i + 2 + 1 + mLen + len) {
        position = mLen + i;
        attrRsp = BUILD_UINT16(afMsg.pData[i]);
        if (mLen) {
            status = afMsg.pData[position + 1];
            if (status != ZCLStatusT::SUCCESS_ZCL) {
                --i;
                len = 0;
                continue;
            }
        }
        type = afMsg.pData[position + 2];
        shiftPos = 0;
        switch (type) {
            case DataTypeT::zcl_doublePrec:
            case DataTypeT::zcl_data64:
            case DataTypeT::zcl_bitmap64:
            case DataTypeT::zcl_int64:
            case DataTypeT::zcl_uint64:
            case DataTypeT::zcl_ieeeAddr:
                len = 8;
                break;
            case DataTypeT::zcl_data56:
            case DataTypeT::zcl_bitmap56:
            case DataTypeT::zcl_int56:
            case DataTypeT::zcl_uint56:
                len = 7;
                break;
            case DataTypeT::zcl_data48:
            case DataTypeT::zcl_bitmap48:
            case DataTypeT::zcl_int48:
            case DataTypeT::zcl_uint48:
                len = 6;
                break;
            case DataTypeT::zcl_data40:
            case DataTypeT::zcl_bitmap40:
            case DataTypeT::zcl_int40:
            case DataTypeT::zcl_uint40:
                len = 5;
                break;
            case DataTypeT::zcl_singlePrec:
            case DataTypeT::zcl_data32:
            case DataTypeT::zcl_bitmap32:
            case DataTypeT::zcl_int32:
            case DataTypeT::zcl_uint32:
            case DataTypeT::zcl_tod:
            case DataTypeT::zcl_date:
            case DataTypeT::zcl_utc:
            case DataTypeT::zcl_bacOid:
                len = 4;
                break;
            case DataTypeT::zcl_data24:
            case DataTypeT::zcl_bitmap24:
            case DataTypeT::zcl_int24:
            case DataTypeT::zcl_uint24:
                len = 3;
                break;
            case DataTypeT::zcl_data16:
            case DataTypeT::zcl_bitmap16:
            case DataTypeT::zcl_int16:
            case DataTypeT::zcl_uint16:
            case DataTypeT::zcl_enum16:
            case DataTypeT::zcl_semiPrec:
            case DataTypeT::zcl_clusterId:
            case DataTypeT::zcl_attrId:
                len = 2;
                break;
            case DataTypeT::zcl_data8:
            case DataTypeT::zcl_boolean:
            case DataTypeT::zcl_bitmap8:
            case DataTypeT::zcl_int8:
            case DataTypeT::zcl_uint8:
            case DataTypeT::zcl_enum8:
                len = 1;
                break;
            case DataTypeT::zcl_octetStr:
            case DataTypeT::zcl_charStr:
                len = 1 + afMsg.pData[position + 2 + 1]; // string
                shiftPos = 1;
                break;
            case DataTypeT::zcl_longOctetStr:
            case DataTypeT::zcl_longCharStr:
                len = 2 + BUILD_UINT16(afMsg.pData[position + 2 + 1]); // long string
                shiftPos = 2;
                break;
            case DataTypeT::zcl_noData:
            case DataTypeT::zcl_unknown:
            default:
                return false;
        }
        result = true;
        if (listAttr.size()) {
            auto eachAttr = std::find_if(listAttr.begin(), listAttr.end(), [attrRsp](const DataAttr_t& e) {
                return e.attribute == attrRsp;
            });
            if (eachAttr ==  listAttr.end()) {
                continue;
            }
            if (eachAttr->value == nullptr) {
                continue;
            }
            for (int j = 0; j < len - shiftPos; ++j) {
                (*(static_cast<uint8_t*>(eachAttr->value) + j)) = afMsg.pData[position + 2 + 1 + shiftPos + j];
            }
            if (shiftPos) {
                (*(static_cast<uint8_t*>(eachAttr->value) + len - shiftPos)) = '\00';
            }
            if (++numElement >= listAttr.size()) {
                break;
            }
        }
        else if (!shiftPos) {
            switch (afMsg.zclId) {
                case ClusterIDT::ZCL_CLUSTER_BASIC:
                    break;
                default:
                    value = 0;
                    for (int j = 0; j < len - shiftPos; ++j) {
                        (*(reinterpret_cast<uint8_t*>(&value) + j)) = afMsg.pData[position + 2 + 1 + shiftPos + j];
                    }
                    /* Create data and response zigbee */
                    if (deviceInfo == nullptr) {
                        deviceInfo = this->createDataGlobal(afMsg, attrRsp, type, value);
                    }
                    else {
                        this->createDataGlobal(afMsg, attrRsp, type, value);
                    }

                    defaultRsp.attribute = attrRsp;
                    defaultRsp.value = value;
                    /* Queue Zigbee Rsp */
                    if (this->thisZigbee().queueDefaultRsp.writeable()) {
                        this->thisZigbee().queueDefaultRsp += defaultRsp;
                    }
                    defaultRsp.isFirst = false;
                    defaultRsp.isSent = true;
                    break;
            }
        }
    }

    if (!listAttr.size()) {
        switch (afMsg.zclId) {
            case ClusterIDT::ZCL_CLUSTER_BASIC:
                break;
            default:
                this->thisZigbee().publishZigbeeData(deviceInfo);
                break;
        }
    }
    return result;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::getDataAttributesRsp(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, vector<DataReadAttrRsp_t> listData) {
    if (!afMsg.pDataLen || afMsg.pData == nullptr) {
        return false;
    }
    if (!listData.size()) {
        return false;
    }

    bool status {false};
    uint16_t attrRsp {0};
    uint8_t numElement {0};

    for (int i = 0; i < afMsg.pDataLen; i += 2) {
        attrRsp = BUILD_UINT16(afMsg.pData[i]);
        auto eachAttr = std::find_if(listData.begin(), listData.end(), [attrRsp](const DataReadAttrRsp_t& e) {
            return e.attribute == attrRsp;
        });
        if (eachAttr == listData.end()) {
            continue;
        }
        eachAttr->found = true;
        status = true;
        if (++numElement >= listData.size()) {
            break;
        }
    }

    if (status) {
        this->thisZigbee().Zigbee::ToZigbee::CommandZigbee::readAttributesResponse(afMsg.srcAddr, afMsg.srcAddr.endpoint,
                                                                                            afMsg.zclId, defaultRsp.manufCode, listData,
                                                                                            AFCommandsT::AF_DATA_CONFIRM, &defaultRsp.transId);
    }

    return status;
}

#endif /* INC_ERA_FROM_GLOBAL_ZIGBEE_HPP_ */
