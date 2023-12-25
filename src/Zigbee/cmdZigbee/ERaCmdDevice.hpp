#ifndef INC_ERA_COMMAND_DEVICE_ZIGBEE_HPP_
#define INC_ERA_COMMAND_DEVICE_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readAttributes(AFAddrType_t& dstAddr,
                                                    EndpointListT srcEndpoint,
                                                    ClusterIDT zclId,
                                                    ManufacturerCodesT manufCode,
                                                    vector<uint16_t> attributes,
                                                    uint8_t cmdWait,
                                                    void* value) {
    vector<uint8_t> payload;
    this->zclHeaderCus = {
        .frameType = FrameTypeT::GLOBAL,
        .manufCode = manufCode,
        .direction = DirectionT::CLIENT_TO_SERVER,
        .disableRsp = true,
        .reservedBits = 0
    };
    for (auto const& var : attributes) {
        payload.push_back(LO_UINT16(var));
        payload.push_back(HI_UINT16(var));
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeaderCus, dstAddr,
                                                    srcEndpoint, {zclId, CommandIdT::READ_ATTR, &payload},
                                                    cmdWait, value,
                                                    MAX_TIMEOUT);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readAttributesResponse(AFAddrType_t& dstAddr,
                                                            EndpointListT srcEndpoint,
                                                            ClusterIDT zclId,
                                                            ManufacturerCodesT manufCode,
                                                            vector<DataReadAttrRsp_t> data,
                                                            uint8_t cmdWait,
                                                            void* _transId) {
    vector<uint8_t> payload;
    this->zclHeaderCus = {
        .frameType = FrameTypeT::GLOBAL,
        .manufCode = manufCode,
        .direction = DirectionT::SERVER_TO_CLIENT,
        .disableRsp = true,
        .reservedBits = 0
    };
    for (auto const& var : data) {
        if (!var.found) {
            continue;
        }
        payload.push_back(LO_UINT16(var.attribute));
        payload.push_back(HI_UINT16(var.attribute));
        payload.push_back(ZCLStatusT::SUCCESS_ZCL);
        payload.push_back(var.type);
        switch (var.type) {
            case DataTypeT::zcl_longOctetStr:
            case DataTypeT::zcl_longCharStr:
                payload.push_back(LO_UINT16(var.data.size()));
                payload.push_back(HI_UINT16(var.data.size()));
                payload.insert(payload.end(), var.data.begin(), var.data.end());
                break;
            case DataTypeT::zcl_octetStr:
            case DataTypeT::zcl_charStr:
            case DataTypeT::zcl_array:
                payload.push_back(var.data.size());
            default:
                payload.insert(payload.end(), var.data.begin(), var.data.end());
                break;
        }
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeaderCus, dstAddr,
                                                    srcEndpoint, {zclId, CommandIdT::READ_ATTR_RSP, &payload},
                                                    cmdWait, nullptr,
                                                    DEFAULT_TIMEOUT, _transId);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeAttributes(AFAddrType_t& dstAddr,
                                                    EndpointListT srcEndpoint,
                                                    ClusterIDT zclId,
                                                    ManufacturerCodesT manufCode,
                                                    vector<DataWriteAttr_t> data,
                                                    uint8_t cmdWait) {
    vector<uint8_t> payload;
    this->zclHeaderCus = {
        .frameType = FrameTypeT::GLOBAL,
        .manufCode = manufCode,
        .direction = DirectionT::CLIENT_TO_SERVER,
        .disableRsp = true,
        .reservedBits = 0
    };
    for (auto const& var : data) {
        payload.push_back(LO_UINT16(var.attribute));
        payload.push_back(HI_UINT16(var.attribute));
        payload.push_back(var.type);
        switch (var.type) {
            case DataTypeT::zcl_longOctetStr:
            case DataTypeT::zcl_longCharStr:
                payload.push_back(LO_UINT16(var.data.size()));
                payload.push_back(HI_UINT16(var.data.size()));
                payload.insert(payload.end(), var.data.begin(), var.data.end());
                break;
            case DataTypeT::zcl_octetStr:
            case DataTypeT::zcl_charStr:
            case DataTypeT::zcl_array:
                payload.push_back(var.data.size());
            default:
                payload.insert(payload.end(), var.data.begin(), var.data.end());
                break;
        }
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeaderCus, dstAddr,
                                                    srcEndpoint, {zclId, CommandIdT::WRITE_ATTR, &payload},
                                                    cmdWait, nullptr,
                                                    MAX_TIMEOUT);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeAttributesResponse(AFAddrType_t& dstAddr,
                                                            EndpointListT srcEndpoint,
                                                            ClusterIDT zclId,
                                                            ManufacturerCodesT manufCode,
                                                            vector<uint16_t> attributes,
                                                            uint8_t cmdWait,
                                                            void* _transId) {
    vector<uint8_t> payload;
    this->zclHeaderCus = {
        .frameType = FrameTypeT::GLOBAL,
        .manufCode = manufCode,
        .direction = DirectionT::SERVER_TO_CLIENT,
        .disableRsp = true,
        .reservedBits = 0
    };
    for (auto const& var : attributes) {
        payload.push_back(ZCLStatusT::SUCCESS_ZCL);
        payload.push_back(LO_UINT16(var));
        payload.push_back(HI_UINT16(var));
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeaderCus, dstAddr,
                                                    srcEndpoint, {zclId, CommandIdT::WRITE_ATTR_RSP, &payload},
                                                    cmdWait, nullptr,
                                                    DEFAULT_TIMEOUT, _transId);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestBind(AFAddrType_t& srcAddr,
                                                AFAddrType_t& dstAddr,
                                                ClusterIDT zclId) {
    if (!srcAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }
    if (IsZeroArray(srcAddr.addr.ieeeAddr)) {
        return ResultT::RESULT_FAIL;
    }
    if (srcAddr.endpoint == EndpointListT::ENDPOINT_NONE) {
        srcAddr.endpoint = EndpointListT::ENDPOINT1;
    }
    if (dstAddr.endpoint == EndpointListT::ENDPOINT_NONE) {
        dstAddr.endpoint = EndpointListT::ENDPOINT1;
    }

    ZnpCommandStatusT status {ZnpCommandStatusT::SUCCESS_STATUS};
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(srcAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(srcAddr.addr.nwkAddr));
    payload.insert(payload.end(), srcAddr.addr.ieeeAddr, srcAddr.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
    payload.push_back(srcAddr.endpoint);
    payload.push_back(LO_UINT16(zclId));
    payload.push_back(HI_UINT16(zclId));
    payload.push_back(dstAddr.addrMode);
    switch (dstAddr.addrMode) {
        case AddressModeT::ADDR_64BIT:
            if (IsZeroArray(dstAddr.addr.ieeeAddr)) {
                return ResultT::RESULT_FAIL;
            }
            payload.insert(payload.end(), dstAddr.addr.ieeeAddr, dstAddr.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
            payload.push_back(dstAddr.endpoint);
            break;
        case AddressModeT::ADDR_GROUP:
            if (!dstAddr.addr.nwkAddr) {
                return ResultT::RESULT_FAIL;
            }
            payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
            payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
            payload.resize(22);
            payload.push_back(EndpointListT::ENDPOINT_BROADCAST);
            break;
        default:
            return ResultT::RESULT_FAIL;
    }

    for (size_t i = 0; i < 2; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_BIND_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_BIND_RSP, &status) == ResultT::RESULT_SUCCESSFUL) {
            return ((status == ZnpCommandStatusT::SUCCESS_STATUS) ? ResultT::RESULT_SUCCESSFUL : ResultT::RESULT_FAIL);
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestUnbind(AFAddrType_t& srcAddr,
                                                AFAddrType_t& dstAddr,
                                                ClusterIDT zclId) {
    if (!srcAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }
    if (IsZeroArray(srcAddr.addr.ieeeAddr)) {
        return ResultT::RESULT_FAIL;
    }
    if (srcAddr.endpoint == EndpointListT::ENDPOINT_NONE) {
        srcAddr.endpoint = EndpointListT::ENDPOINT1;
    }
    if (dstAddr.endpoint == EndpointListT::ENDPOINT_NONE) {
        dstAddr.endpoint = EndpointListT::ENDPOINT1;
    }

    ZnpCommandStatusT status {ZnpCommandStatusT::SUCCESS_STATUS};
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(srcAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(srcAddr.addr.nwkAddr));
    payload.insert(payload.end(), srcAddr.addr.ieeeAddr, srcAddr.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
    payload.push_back(srcAddr.endpoint);
    payload.push_back(LO_UINT16(zclId));
    payload.push_back(HI_UINT16(zclId));
    payload.push_back(dstAddr.addrMode);
    switch (dstAddr.addrMode) {
        case AddressModeT::ADDR_64BIT:
            if (IsZeroArray(dstAddr.addr.ieeeAddr)) {
                return ResultT::RESULT_FAIL;
            }
            payload.insert(payload.end(), dstAddr.addr.ieeeAddr, dstAddr.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
            payload.push_back(dstAddr.endpoint);
            break;
        case AddressModeT::ADDR_GROUP:
            if (!dstAddr.addr.nwkAddr) {
                return ResultT::RESULT_FAIL;
            }
            payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
            payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
            payload.resize(22);
            payload.push_back(EndpointListT::ENDPOINT_BROADCAST);
            break;
        default:
            return ResultT::RESULT_FAIL;
    }

    for (size_t i = 0; i < 2; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_UNBIND_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_UNBIND_RSP, &status) == ResultT::RESULT_SUCCESSFUL) {
            return ((status == ZnpCommandStatusT::SUCCESS_STATUS) ? ResultT::RESULT_SUCCESSFUL : ResultT::RESULT_FAIL);
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestSimpleDesc(AFAddrType_t& dstAddr, size_t limit, void* value) {
    if (value == nullptr) {
        value = this->device;
    }
    uint8_t tick {0};
    if (this->device->epTick != this->device->epCount - 1) {
        tick = this->device->epTick + 1;
    }
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(dstAddr.endpoint);
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_SIMPLE_DESC_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_SIMPLE_DESC_RSP, value) == ResultT::RESULT_SUCCESSFUL) { /* Descriptor of the dest device's Endpoint */
            if (this->device->epTick == tick) {
                return ResultT::RESULT_SUCCESSFUL;
            }
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestNodeDesc(AFAddrType_t& dstAddr, size_t limit, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_NODE_DESC_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_NODE_DESC_RSP, value) == ResultT::RESULT_SUCCESSFUL) { /* Inquire about the Node Descriptor information of the dest device */
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::extRouterDiscovery(AFAddrType_t& dstAddr, const OptionsAFT options, const uint8_t radius) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(options);
    payload.push_back(radius);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_EXT_ROUTE_DISC,
                                                    TypeT::SRSP, ZDOCommandsT::ZDO_EXT_ROUTE_DISC); /* ZDO route extension message */
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestListEndpoint(AFAddrType_t& dstAddr, uint16_t nwkAddrOfInterest, size_t limit, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(LO_UINT16(nwkAddrOfInterest));
    payload.push_back(HI_UINT16(nwkAddrOfInterest));
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_ACTIVE_EP_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_ACTIVE_EP_RSP, value) == ResultT::RESULT_SUCCESSFUL) { /* Request a list of active endpoint from the dest device */
            if (this->device->epCount) {
                return ResultT::RESULT_SUCCESSFUL;
            }
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::removeDevice(AFAddrType_t& dstAddr, bool rejoin, bool removeChildren, bool force) {
    if (!dstAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }

    uint8_t option = (rejoin ? 0b1 : 0b0);
    option |= (removeChildren ? 0b10 : 0x00);
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    if (force) {
        payload.insert(payload.end(), dstAddr.addr.ieeeAddr, dstAddr.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
    }
    else {
        payload.resize(10);
    }
    payload.push_back(option);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_MGMT_LEAVE_REQ,
                                                    TypeT::SRSP, ZDOCommandsT::ZDO_MGMT_LEAVE_REQ);
}

template <class ToZigbee>
void ERaCommandZigbee<ToZigbee>::removeAllDevice(bool rejoin, bool removeChildren, bool force) {
    uint8_t option = (rejoin ? 0b1 : 0b0);
    option |= (removeChildren ? 0b10 : 0x00);

    vector<uint8_t> payload;
    for (size_t i = 0; i < this->coordinator->deviceCount; ++i) {
        if (!this->coordinator->deviceIdent[i].address.addr.nwkAddr) {
            continue;
        }
        payload.clear();
        payload.push_back(LO_UINT16(this->coordinator->deviceIdent[i].address.addr.nwkAddr));
        payload.push_back(HI_UINT16(this->coordinator->deviceIdent[i].address.addr.nwkAddr));
        if (force) {
            payload.insert(payload.end(), this->coordinator->deviceIdent[i].address.addr.ieeeAddr, this->coordinator->deviceIdent[i].address.addr.ieeeAddr + LENGTH_EXTADDR_IEEE);
        }
        else {
            payload.resize(10);
        }
        payload.push_back(option);
        this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_MGMT_LEAVE_REQ,
                                                TypeT::AREQ, ZDOCommandsT::ZDO_MGMT_LEAVE_RSP);
    }
}

#endif /* INC_ERA_COMMAND_DEVICE_ZIGBEE_HPP_ */
