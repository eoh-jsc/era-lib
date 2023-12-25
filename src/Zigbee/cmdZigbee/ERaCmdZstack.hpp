#ifndef INC_ERA_COMMAND_ZSTACK_ZIGBEE_HPP_
#define INC_ERA_COMMAND_ZSTACK_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
void ERaCommandZigbee<ToZigbee>::skipBootloader() {
    this->thisToZigbee().sendByte(this->SkipBootByte);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::pingSystem(size_t limit, uint32_t timeout, void (*feed)(void)) {
    vector<uint8_t> payload;
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_PING,
                                                    TypeT::SRSP, SYSCommandsT::SYS_PING,
                                                    nullptr, timeout) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        this->skipBootloader();
        ERaDelay(1000);
        if (feed != nullptr) {
            feed();
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::versionSystem(size_t limit, void* value) {
    vector<uint8_t> payload;
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_VERSION,
                                                    TypeT::SRSP, SYSCommandsT::SYS_VERSION, value) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::getExtAddrSystem(size_t limit, void* value) {
    vector<uint8_t> payload;
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_GET_EXTADDR,
                                                    TypeT::SRSP, SYSCommandsT::SYS_GET_EXTADDR, value) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::nwkKeyReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::NWKKEY, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::znpHasConfiguredReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(((this->coordinator->product == ZnpVersionT::zStack12) ? NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK1 : NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK3), 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::extAddrReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::EXTADDR, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::nibReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(NvItemsIdsT::NIB, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::waitNibReadOSALZstack(size_t limit, void* value) {
    for (size_t i = 0; i < limit; ++i) {
        if (this->readItemZstack(NvItemsIdsT::NIB, 0, value) == ResultT::RESULT_SUCCESSFUL) {
            if ((this->coordinator->nIB.nwkPanId != 0 && this->coordinator->nIB.nwkPanId != 0xFFFF &&
                this->coordinator->nIB.nwkLogicalChannel != 0) || this->coordinator->product == zStack12) {
                return ResultT::RESULT_SUCCESSFUL;
            }
            ERaDelay(3000);
        }
        else {
            ERaDelay(1000);
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::channelListReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(NvItemsIdsT::CHANLIST, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::panIdReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(NvItemsIdsT::PANID, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::extendedPanIdReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::EXTENDED_PAN_ID, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::activeKeyReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::NWK_ACTIVE_KEY_INFO, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::alternKeyReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::NWK_ALTERN_KEY_INFO, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::apsExtPanIdReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::APS_USE_EXT_PANID, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::apsLinkKeyReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(NvItemsIdsT::APS_LINK_KEY_DATA_START, 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::enablePrecfgkeysReadOSALZstack(size_t limit, void* value) {
    // check elements - update soon
    return this->readItemZstacks(NvItemsIdsT::PRECFGKEYS_ENABLE, 0, limit, value); /* check key!!! */
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysReadConfigZstack(size_t limit, void* value) { // for zigbee 1.2
    this->coordinator->nvItem = NvItemsIdsT::PRECFGKEY;

    vector<uint8_t> payload;
    payload.push_back(NvItemsIdsT::PRECFGKEY); /* Network key */
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SAPI_INTER, ZBCommandsT::ZB_READ_CONFIGURATION,
                                                    TypeT::SRSP, ZBCommandsT::ZB_READ_CONFIGURATION) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysReadOSALZstack(size_t limit, void* value) { // for zigbee 3.0
    return this->readItemZstacks(NvItemsIdsT::PRECFGKEY, 0, limit, value); /* Network key */
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysReadZstack(size_t limit, void* value) {
    if (this->coordinator->product == ZnpVersionT::zStack12) {
        return this->precfgkeysReadConfigZstack(limit, value);
    }
    else {
        return this->precfgkeysReadOSALZstack(limit, value);
    }
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::touchlinkKeyReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(((this->coordinator->product == ZnpVersionT::zStack12) ? NvItemsIdsT::LEGACY_TCLK_TABLE_START_12 : ((this->coordinator->product == ZnpVersionT::zStack30x) ? NvItemsIdsT::LEGACY_TCLK_TABLE_START : NvItemsIdsT::EX_TCLK_TABLE)), 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::materialTableReadOSALZstack(size_t limit, void* value) {
    return this->readItemZstacks(((this->coordinator->product != ZnpVersionT::zStack3x0) ? NvItemsIdsT::LEGACY_NWK_SEC_MATERIAL_TABLE_START : NvItemsIdsT::EX_NWK_SEC_MATERIAL_TABLE), 0, limit, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::getDeviceInfoZstack(size_t limit, void* value) {
    vector<uint8_t> payload;
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::UTIL_INTER, UTILCommandsT::UTIL_GET_DEVICE_INFO,
                                                    TypeT::SRSP, UTILCommandsT::UTIL_GET_DEVICE_INFO,
                                                    value) == ResultT::RESULT_SUCCESSFUL) { /* Get list device connected */
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::stateStartupAppZstack(size_t limit, void* value) {
    if (this->coordinator->states == DevStatesT::ZB_COORD) {
        return ResultT::RESULT_SUCCESSFUL;
    }

    vector<uint8_t> payload;
    payload.push_back(0x64); /* Start delay - get state 0x09(STARTED_COORDINATOR) is Okey */
    payload.push_back(0x00);
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_STARTUP_FROM_APP,
                                                    TypeT::AREQ, ((this->coordinator->product == ZnpVersionT::zStack12) ? (uint8_t)ZDOCommandsT::ZDO_STATE_CHANGE_IND : (uint8_t)APPCFCommandsT::APP_CNF_BDB_COMMISSIONING_NOTIFICATION),
                                                    value, MAX_TIMEOUT,
                                                    ((this->coordinator->product == ZnpVersionT::zStack12) ? SubsystemT::RESERVED_INTER : SubsystemT::APP_CNF_INTER)) == ResultT::RESULT_SUCCESSFUL) {
            if (this->coordinator->deviceState == DeviceStateListT::STARTED_COORDINATOR) {
                return ResultT::RESULT_SUCCESSFUL;
            }
            for (size_t j = 0; j < 1000; ++j) {
                // loop check
                this->thisToZigbee().handleZigbeeData();
                if (this->coordinator->deviceState == DeviceStateListT::STARTED_COORDINATOR) {
                    return ResultT::RESULT_SUCCESSFUL;
                }
                ERA_ZIGBEE_YIELD();
            }
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestListEndpointZstack(uint16_t nwkAddr, uint16_t nwkAddrOfinterest, size_t limit, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(nwkAddr));
    payload.push_back(HI_UINT16(nwkAddr));
    payload.push_back(LO_UINT16(nwkAddrOfinterest));
    payload.push_back(HI_UINT16(nwkAddrOfinterest));
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_ACTIVE_EP_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_ACTIVE_EP_RSP,
                                                    value) == ResultT::RESULT_SUCCESSFUL) { /* Request nwkAddr coordinator and list endpoint coordinator */
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::findGroupExtZDO(EndpointListT endpoint, uint16_t groupId, size_t limit, void* value) {
    vector<uint8_t> payload;
    payload.push_back(endpoint);
    payload.push_back(LO_UINT16(groupId));
    payload.push_back(HI_UINT16(groupId));
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_EXT_FIND_GROUP,
                                                    TypeT::SRSP, ZDOCommandsT::ZDO_EXT_FIND_GROUP,
                                                    value) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::addGroupExtZDO(EndpointListT endpoint, uint16_t groupId, char* groupName) {
    vector<uint8_t> payload;
    payload.push_back(endpoint);
    payload.push_back(LO_UINT16(groupId));
    payload.push_back(HI_UINT16(groupId));
    if (groupName != nullptr) {
        payload.push_back(strlen(groupName));
        payload.insert(payload.end(), groupName, groupName + strlen(groupName));
    }
    else {
        payload.push_back(0x00);
    }
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_EXT_ADD_GROUP,
                                                    TypeT::SRSP, ZDOCommandsT::ZDO_EXT_ADD_GROUP);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::addGroupExtZDOZstack(EndpointListT endpoint, uint16_t groupId, void* value, bool format) {
    if (!groupId) {
        return ResultT::RESULT_FAIL;
    }
    if (endpoint == EndpointListT::ENDPOINT_NONE) {
        endpoint == EndpointListT::ENDPOINT1;
    }
    if (value == nullptr) {
        value = this->coordinator;
    }

    if (format) {
        return this->addGroupExtZDO(endpoint, groupId, nullptr);
    }
    if (this->findGroupExtZDO(endpoint, groupId, 2, value) != ResultT::RESULT_SUCCESSFUL) {
        return this->addGroupExtZDO(endpoint, groupId, nullptr);
    }
    else {
        if (this->coordinator->extGroup != groupId) {
            return this->addGroupExtZDO(endpoint, groupId, nullptr);
        }
    }
    return ResultT::RESULT_SUCCESSFUL;
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::requestSimpleDescZstack(const InitEndpoint_t(&epList)[size], void* value) {
    vector<uint8_t> payload;
    payload.resize(5);
    for (int i = 0; i < size; ++i) {
        payload.at(4) = epList[i].endpoint;
        for (size_t j = 0; j < 5; ++j) {
            if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                        SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_SIMPLE_DESC_REQ,
                                                        TypeT::AREQ, ZDOCommandsT::ZDO_SIMPLE_DESC_RSP,
                                                        value) == ResultT::RESULT_SUCCESSFUL) {
                return ResultT::RESULT_SUCCESSFUL;
            }
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::afRegisterEndpointZstack(const InitEndpoint_t& epInfo) {
    vector<uint8_t> payload;
    payload.push_back(epInfo.endpoint);
    payload.push_back(LO_UINT16(epInfo.profileId));
    payload.push_back(HI_UINT16(epInfo.profileId));
    payload.push_back(LO_UINT16(epInfo.deviceId));
    payload.push_back(HI_UINT16(epInfo.deviceId));
    payload.push_back(epInfo.appDevVer);
    payload.push_back(epInfo.latency);
    payload.push_back(epInfo.numZclIn);
    for (size_t i = 0; i < epInfo.numZclIn; ++i) {
        payload.push_back(LO_UINT16(epInfo.zclIdIn[i]));
        payload.push_back(HI_UINT16(epInfo.zclIdIn[i]));
    }
    payload.push_back(epInfo.numZclOut);
    for (size_t i = 0; i < epInfo.numZclOut; ++i) {
        payload.push_back(LO_UINT16(epInfo.zclIdOut[i]));
        payload.push_back(HI_UINT16(epInfo.zclIdOut[i]));
    }
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::AF_INTER, AFCommandsT::AF_REGISTER,
                                                    TypeT::SRSP, AFCommandsT::AF_REGISTER);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::afRegisterEndpointListZstack(const InitEndpoint_t(&epList)[size]) {
    for (int i = 0; i < size; ++i) {
        this->afRegisterEndpointZstack(epList[i]);
    }
    return ResultT::RESULT_SUCCESSFUL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestNwkAddrZstack(AFAddrType_t& dstAddr, uint8_t reqType, uint8_t startIndex, void* value) {
    if (value == nullptr) {
        value = &dstAddr;
    }
    if (IsZeroArray(dstAddr.addr.ieeeAddr)) {
        return ResultT::RESULT_FAIL;
    }
    vector<uint8_t> payload;
    payload.insert(payload.end(), dstAddr.addr.ieeeAddr, dstAddr.addr.ieeeAddr + sizeof(dstAddr.addr.ieeeAddr));
    payload.push_back(reqType);
    payload.push_back(startIndex); /* Single Device response and index */
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_NWK_ADDR_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_NWK_ADDR_RSP,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestIEEEAddrZstack(AFAddrType_t& dstAddr, uint8_t reqType, uint8_t startIndex, void* value) {
    if (value == nullptr) {
        value = &dstAddr;
    }
    if (!dstAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(reqType);
    payload.push_back(startIndex); /* Single Device response and index */
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_IEEE_ADDR_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_IEEE_ADDR_RSP,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestInfoNwkExtZstack(size_t limit, void* value) {
    vector<uint8_t> payload;
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_EXT_NWK_INFO,
                                                    TypeT::SRSP, ZDOCommandsT::ZDO_EXT_NWK_INFO,
                                                    value) == ResultT::RESULT_SUCCESSFUL) { /* ZDO extension network message */
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::requestResetZstack(ResetTypeT type, size_t limit, void* value) {
    vector<uint8_t> payload;
    payload.push_back(type);
    for (size_t i = 0; i < limit; ++i) {
        if (this->thisToZigbee().createCommandBuffer(payload, TypeT::AREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_RESET_REQ,
                                                    TypeT::AREQ, SYSCommandsT::SYS_RESET_IND,
                                                    value, MAX_TIMEOUT * 2) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
        ERaDelay(1000);
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::startupOptionWriteOSALZstack(uint8_t option) {
    vector<uint8_t> payload;
    payload.push_back(option);
    return this->writeItemZstack(NvItemsIdsT::STARTUP_OPTION, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::logicalTypeWriteOSALZstack(TypeDeviceT type) {
    vector<uint8_t> payload;
    payload.push_back(type);
    return this->writeItemZstack(NvItemsIdsT::LOGICAL_TYPE, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::enablePrecfgkeysWriteOSALZstack(bool distribute) {
    vector<uint8_t> payload;
    payload.push_back(distribute ? 0x01 : 0x00);
    return this->writeItemZstack(NvItemsIdsT::PRECFGKEYS_ENABLE, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::zdoDirectWriteOSALZstack(bool enable) {
    vector<uint8_t> payload;
    payload.push_back(enable ? 0x01 : 0x00);
    return this->writeItemZstack(NvItemsIdsT::ZDO_DIRECT_CB, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::activeKeyWriteOSALZstack(const uint8_t(&key)[size]) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.insert(payload.end(), key, key + size);
    return this->writeItemZstack(NvItemsIdsT::NWK_ACTIVE_KEY_INFO, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::alternKeyWriteOSALZstack(const uint8_t(&key)[size]) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.insert(payload.end(), key, key + size);
    return this->writeItemZstack(NvItemsIdsT::NWK_ALTERN_KEY_INFO, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::apsExtPanIdWriteOSALZstack(const uint8_t(&extPanId)[size]) {
    vector<uint8_t> payload;
    payload.insert(payload.end(), extPanId, extPanId + size);
    return this->writeItemZstack(NvItemsIdsT::APS_USE_EXT_PANID, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::materialTableWriteOSALZstack(const uint8_t* pData, size_t pDataLen) {
    if (pData == nullptr) {
        return ResultT::RESULT_FAIL;
    }
    vector<uint8_t> payload;
    payload.insert(payload.end(), pData, pData + pDataLen);
    return this->writeItemZstack(((this->coordinator->product == ZnpVersionT::zStack3x0) ? NvItemsIdsT::EX_NWK_SEC_MATERIAL_TABLE : NvItemsIdsT::LEGACY_NWK_SEC_MATERIAL_TABLE_START), 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::channelListWriteOSALZstack(const ZBChannelT channel) {
    vector<uint8_t> payload;
    payload.push_back(BREAK_UINT32(channel, 0));
    payload.push_back(BREAK_UINT32(channel, 1));
    payload.push_back(BREAK_UINT32(channel, 2));
    payload.push_back(BREAK_UINT32(channel, 3));
    return this->writeItemZstack(NvItemsIdsT::CHANLIST, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::panIdWriteOSALZstack(const uint16_t panId) {
    if (!panId || panId > 0xFFF0) {
        return ResultT::RESULT_FAIL;
    }
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(panId));
    payload.push_back(HI_UINT16(panId));
    return this->writeItemZstack(NvItemsIdsT::PANID, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::extendedPanIdWriteOSALZstack(const uint8_t(&extPanId)[size]) {
    vector<uint8_t> payload;
    payload.insert(payload.end(), extPanId, extPanId + size);
    return this->writeItemZstack(NvItemsIdsT::EXTENDED_PAN_ID, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysWriteConfigZstack(const uint8_t(&key)[size]) { /* For 1.2 */
    vector<uint8_t> payload;
    payload.push_back(NvItemsIdsT::PRECFGKEY);
    payload.push_back(size);
    payload.insert(payload.end(), key, key + size);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SAPI_INTER, ZBCommandsT::ZB_WRITE_CONFIGURATION,
                                                    TypeT::SRSP, ZBCommandsT::ZB_WRITE_CONFIGURATION);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysWriteOSALZstack(const uint8_t(&key)[size]) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.insert(payload.end(), key, key + size);
    return this->writeItemZstack(NvItemsIdsT::PRECFGKEY, 0, payload);
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::precfgkeysWriteZstack(const uint8_t(&key)[size]) {
    if (this->coordinator->product == ZnpVersionT::zStack12) {
        return this->precfgkeysWriteConfigZstack(key);
    }
    else {
        return this->precfgkeysWriteOSALZstack(key);
    }
}

template <class ToZigbee>
template <int size>
ResultT ERaCommandZigbee<ToZigbee>::touchlinkKeyWriteOSALZstack(const uint8_t(&key)[size]) { /* For 1.2 */
    if (this->coordinator->product != ZnpVersionT::zStack12) {
        return ResultT::RESULT_FAIL;
    }
    vector<uint8_t> payload;
    payload.insert(payload.end(), key, key + size);
    return this->writeItemZstack(NvItemsIdsT::LEGACY_TCLK_TABLE_START_12, 0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::znpHasConfiguredWriteOSALInitZstack() {
    vector<uint8_t> payload;
    payload.push_back(0x00);
    return this->writeOSALItemInit(((this->coordinator->product == ZnpVersionT::zStack12) ?
                                    NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK1 : NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK3),
                                    0x0001, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::znpHasConfiguredWriteOSALZstack() {
    vector<uint8_t> payload;
    payload.push_back(FlagsZigbeeT::FLAG_ZIGBEE_HAS_CONFIGURED);
    return this->writeItemZstack(((this->coordinator->product == ZnpVersionT::zStack12) ?
                                    NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK1 : NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK3),
                                    0, payload);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::touchlinkStartCommZstack() { /* For 3.0 */
    return this->startCommissioningZstack(APPCFCommModeT::COMM_MODE_TOUCHLINK);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::netSteeringStartCommZstack() { /* For 3.0 */
    return this->startCommissioningZstack(APPCFCommModeT::COMM_MODE_NETWORK_STEERING);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::netFormationStartCommZstack() { /* For 3.0 */
    return this->startCommissioningZstack(APPCFCommModeT::COMM_MODE_NETWORK_FORMATION);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::findAndBindingStartCommZstack() { /* For 3.0 */
    return this->startCommissioningZstack(APPCFCommModeT::COMM_MODE_FINDING_AND_BINDING);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::startCommissioningZstack(uint8_t mode) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.push_back(mode);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::APP_CNF_INTER, APPCFCommandsT::APP_CNF_BDB_START_COMMISSIONING,
                                                    TypeT::SRSP, APPCFCommandsT::APP_CNF_BDB_START_COMMISSIONING);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::setBDBChannelZstack(ZBChannelT channel, bool isPrimary) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.push_back(isPrimary);
    payload.push_back(BREAK_UINT32(channel, 0));
    payload.push_back(BREAK_UINT32(channel, 1));
    payload.push_back(BREAK_UINT32(channel, 2));
    payload.push_back(BREAK_UINT32(channel, 3));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::APP_CNF_INTER, APPCFCommandsT::APP_CNF_BDB_SET_CHANNEL,
                                                    TypeT::SRSP, APPCFCommandsT::APP_CNF_BDB_SET_CHANNEL);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::setTuningOperationZstack(uint8_t operation, uint8_t value) { /* For 3.0 */
    vector<uint8_t> payload;
    payload.push_back(operation);
    payload.push_back(value);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_STACK_TUNE,
                                                    TypeT::SRSP, SYSCommandsT::SYS_STACK_TUNE);
}

#endif /* INC_ERA_COMMAND_ZSTACK_ZIGBEE_HPP_ */
