#ifndef INC_ERA_FROM_STACK_ZIGBEE_HPP_
#define INC_ERA_FROM_STACK_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processNodeDescriptor(vector<uint8_t>& data, void* value) {
    if (data.size() < 3) {
        return;
    }
    if (data.at(2) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    uint16_t srcAddr = BUILD_UINT16(data.at(0));
    uint16_t nwkAddr = BUILD_UINT16(data.at(3));
    if (srcAddr != nwkAddr) {
        return;
    }
    switch (nwkAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            break;
        default:
            this->device->typeDevice = static_cast<TypeDeviceT>(data.at(5) & 0x07);
            this->device->manufCode = static_cast<ManufacturerCodesT>(BUILD_UINT16(data.at(8)));
            this->device->hasManufCode = true;
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processSimpleDescriptor(vector<uint8_t>& data, void* value) {
    if (data.size() < 3) {
        return;
    }
    if (data.at(2) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    uint16_t srcAddr = BUILD_UINT16(data.at(0));
    uint16_t nwkAddr = BUILD_UINT16(data.at(3));
    if (srcAddr != nwkAddr) {
        return;
    }
    InfoEndpoint_t* epInfo = nullptr;
    switch (nwkAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            epInfo = std::find_if(std::begin(this->coordinator->epList), std::end(this->coordinator->epList), find_endpoint_t(static_cast<EndpointListT>(data.at(6))));
            if (epInfo == std::end(this->coordinator->epList)) {
                break;
            }
            epInfo->profileId = static_cast<ProfileIDT>(BUILD_UINT16(data.at(7)));
            epInfo->deviceId = static_cast<DeviceIDT>(BUILD_UINT16(data.at(9)));
            epInfo->deviceVer = data.at(11);
            epInfo->inZclCount = data.at(12);
            for (size_t i = 0; i < epInfo->inZclCount; ++i) {
                epInfo->inZclIdList[i] = static_cast<ClusterIDT>(BUILD_UINT16(data.at(13 + 2 * i)));
            }
            epInfo->outZclCount = data.at(13 + 2 * epInfo->inZclCount);
            for (size_t i = 0; i < epInfo->outZclCount; ++i) {
                epInfo->outZclIdList[i] = static_cast<ClusterIDT>(BUILD_UINT16(data.at(14 + 2 * epInfo->inZclCount + 2 * i)));
            }
            if (++this->coordinator->epTick >= this->coordinator->epCount) {
                this->coordinator->epTick = 0;
            }
            break;
        default:
            epInfo = std::find_if(std::begin(this->device->epList), std::end(this->device->epList), find_endpoint_t(static_cast<EndpointListT>(data.at(6))));
            if (epInfo == std::end(this->device->epList)) {
                break;
            }
            epInfo->profileId = static_cast<ProfileIDT>(BUILD_UINT16(data.at(7)));
            epInfo->deviceId = static_cast<DeviceIDT>(BUILD_UINT16(data.at(9)));
            epInfo->deviceVer = data.at(11);
            epInfo->inZclCount = data.at(12);
            for (size_t i = 0; i < epInfo->inZclCount; ++i) {
                epInfo->inZclIdList[i] = static_cast<ClusterIDT>(BUILD_UINT16(data.at(13 + 2 * i)));
                if (epInfo->inZclIdList[i] == ClusterIDT::ZCL_CLUSTER_BASIC) {
                    epInfo->isBasic = true;
                }
                if (epInfo->inZclIdList[i] == ClusterIDT::ZCL_CLUSTER_POLL_CONTROL) {
                    epInfo->pollControl = true;
                }
                if (epInfo->inZclIdList[i] == ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE) {
                    epInfo->ias = true;
                }
            }
            epInfo->outZclCount = data.at(13 + 2 * epInfo->inZclCount);
            for (size_t i = 0; i < epInfo->outZclCount; ++i) {
                epInfo->outZclIdList[i] = static_cast<ClusterIDT>(BUILD_UINT16(data.at(14 + 2 * epInfo->inZclCount + 2 * i)));
                if (epInfo->outZclIdList[i] == ClusterIDT::ZCL_CLUSTER_BASIC) {
                    epInfo->isBasic = true;
                }
                if (epInfo->outZclIdList[i] == ClusterIDT::ZCL_CLUSTER_POLL_CONTROL) {
                    epInfo->pollControl = true;
                }
                if (epInfo->outZclIdList[i] == ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE) {
                    epInfo->ias = true;
                }
            }
            if (++this->device->epTick >= this->device->epCount) {
                this->device->epTick = 0;
            }
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processActiveEndpoint(vector<uint8_t>& data, void* value) {
    if (data.size() < 3) {
        return;
    }
    if (data.at(2) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    uint16_t srcAddr = BUILD_UINT16(data.at(0));
    uint16_t nwkAddr = BUILD_UINT16(data.at(3));
    if (srcAddr != nwkAddr) {
        return;
    }
    InfoEndpoint_t* epInfo = nullptr;
    switch (nwkAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            this->coordinator->epCount = data.at(5);
            for (size_t i = 0; i < this->coordinator->epCount; ++i) {
                this->coordinator->epList[i].endpoint = ((data.at(6 + i) != EndpointListT::ENDPOINT_NONE)
                                                        ? static_cast<EndpointListT>(data.at(6 + i))
                                                        : EndpointListT::ENDPOINT1);
            }
            std::sort(std::begin(this->coordinator->epList), std::begin(this->coordinator->epList) + this->coordinator->epCount, [](const InfoEndpoint_t& lEp, const InfoEndpoint_t& rEp) {
                return lEp.endpoint < rEp.endpoint;
            });
            break;
        default:
            this->device->epCount = data.at(5);
            for (size_t i = 0; i < this->device->epCount; ++i) {
                this->device->epList[i].endpoint = ((data.at(6 + i) != EndpointListT::ENDPOINT_NONE)
                                                        ? static_cast<EndpointListT>(data.at(6 + i))
                                                        : EndpointListT::ENDPOINT1);
            }
            std::sort(std::begin(this->device->epList), std::begin(this->device->epList) + this->device->epCount, [](const InfoEndpoint_t& lEp, const InfoEndpoint_t& rEp) {
                return lEp.endpoint < rEp.endpoint;
            });
            break;
    }
    ERA_FORCE_UNUSED(epInfo);
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processBindUnbind(vector<uint8_t>& data, void* value) {
    if (data.size() < 3) {
        return;
    }
    if (value == nullptr) {
        return;
    }
    uint16_t srcAddr = BUILD_UINT16(data.at(0));
    switch (srcAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            break;
        default:
            *static_cast<ZnpCommandStatusT*>(value) = static_cast<ZnpCommandStatusT>(data.at(2));
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processTCDeviceIndication(vector<uint8_t>& data, void* value) {
    if (data.size() < 12) {
        return;
    }
    uint16_t nwkAddr = BUILD_UINT16(data.at(0));
    uint16_t parentAddr = BUILD_UINT16(data.at(10));
    uint8_t ieeeAddr[LENGTH_EXTADDR_IEEE] {0};
    CopyToArray(data.at(2), ieeeAddr, LENGTH_EXTADDR_IEEE);
    if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_JOINED)) {
        if (CompareArray(this->device->address.addr.ieeeAddr, ieeeAddr) &&
            (this->device->address.addr.nwkAddr != nwkAddr)) {
            this->device->address.addr.nwkAddr = nwkAddr;
        }
        return;
    }
    if (!ZigbeeState::is(ZigbeeStateT::STATE_ZB_PERMIT_JOIN)) {
        return;
    }
    if (!this->coordinator->permitJoin.enable) {
        return;
    }
    switch (nwkAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            break;
        default:
            this->device->reset();
            this->device->isJoing = true;
            this->device->address.addr.nwkAddr = nwkAddr;
            CopyToArray(data.at(2), this->device->address.addr.ieeeAddr, LENGTH_EXTADDR_IEEE);
            this->device->parentAddr = parentAddr;

            if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_PERMIT_JOIN)) {
                ZigbeeState::set(ZigbeeStateT::STATE_ZB_DEVICE_JOINED);
            }

            this->createDeviceEvent(DeviceEventT::DEVICE_EVENT_JOINED);
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processZDOState(vector<uint8_t>& data, void* value) {
    if (!data.size()) {
        return;
    }
    this->coordinator->deviceState = static_cast<DeviceStateListT>(data.at(0));
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processDeviceAnnounce(vector<uint8_t>& data, void* value) {
    if (data.size() < 13) {
        return;
    }
    this->device->annceDevice.dstAddr.addr.nwkAddr = BUILD_UINT16(data.at(2));
    CopyToArray(data.at(4), this->device->annceDevice.dstAddr.addr.ieeeAddr, LENGTH_EXTADDR_IEEE);
    this->device->annceDevice.alternatePanId = static_cast<bool>((data.at(12)) & 0x01);
    this->device->annceDevice.type = static_cast<TypeAnnceDeviceT>((data.at(12) >> 1) & 0x01);
    this->device->annceDevice.power = static_cast<PowerSourceT>((data.at(12) >> 2) & 0x01);
    this->device->annceDevice.isIdle = static_cast<bool>((data.at(12) >> 3) & 0x01);
    if (this->device->annceDevice.type == TypeAnnceDeviceT::ANNCE_ENDDEVICE) {
        this->thisZigbee().Zigbee::ToZigbee::CommandZigbee::extRouterDiscovery(this->device->address,
                                    this->thisZigbee().Options, this->thisZigbee().Radius);
    }
    if (!CompareArray(this->device->address.addr.ieeeAddr, this->device->annceDevice.dstAddr.addr.ieeeAddr)) {
        this->createDeviceEvent(DeviceEventT::DEVICE_EVENT_ANNOUNCE);
        return;
    }
    if (this->device->address.addr.nwkAddr != this->device->annceDevice.dstAddr.addr.nwkAddr) {
        this->device->address.addr.nwkAddr = this->device->annceDevice.dstAddr.addr.nwkAddr;
    }
    if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_JOINED)) {
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING);
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processDeviceLeave(vector<uint8_t>& data, void* value) {
    if (data.size() < 13) {
        return;
    }
    AFAddrType_t srcAddr;
    srcAddr.addr.nwkAddr = BUILD_UINT16(data.at(0));
    CopyToArray(data.at(2), srcAddr.addr.ieeeAddr, LENGTH_EXTADDR_IEEE);
    bool request = data.at(10);
    bool remove = data.at(11);
    bool rejoin = data.at(12);
    this->thisZigbee().Zigbee::ToZigbee::CommandZigbee::removeDevice(srcAddr, false, false, false);
    this->createDeviceEvent(DeviceEventT::DEVICE_EVENT_LEAVE, &srcAddr);
    ERA_FORCE_UNUSED(request);
    ERA_FORCE_UNUSED(remove);
    ERA_FORCE_UNUSED(rejoin);
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReadOsalNVItems(vector<uint8_t>& data, void* value) {
    if (!data.size()) {
        return;
    }
    if (data.at(0) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    uint8_t length = data.at(1);
    switch (this->coordinator->nvItem) {
        case NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK1:
        case NvItemsIdsT::ZNP_HAS_CONFIGURED_ZSTACK3:
            if (length == 0x01) {
                this->coordinator->hasConfigured = data.at(2);
            }
            break;
        case NvItemsIdsT::PRECFGKEYS_ENABLE:
            if (length == 0x01) {
                this->coordinator->enableKey = data.at(2);
            }
            break;
        case NvItemsIdsT::PANID:
            if (length == 0x02) {
                this->coordinator->address.panId = BUILD_UINT16(data.at(2));
            }
            break;
        case NvItemsIdsT::CHANLIST:
            if (length == 0x04) {
                this->coordinator->channel = static_cast<ZBChannelT>(BUILD_UINT32(data.at(2)));
            }
            break;
        case NvItemsIdsT::EXTENDED_PAN_ID:
            if (length == LENGTH_EXTADDR_IEEE) {
                CopyToArray(data.at(2), this->coordinator->extPanId, LENGTH_EXTADDR_IEEE);
            }
            break;
        case NvItemsIdsT::APS_USE_EXT_PANID:
            if (length == LENGTH_EXTADDR_IEEE) {
                CopyToArray(data.at(2), this->coordinator->apsExtPanId, LENGTH_EXTADDR_IEEE);
            }
            break;
        case NvItemsIdsT::EXTADDR:
            if (length == LENGTH_EXTADDR_IEEE) {
                CopyToArray(data.at(2), this->coordinator->extAddr, LENGTH_EXTADDR_IEEE);
            }
            break;
        case NvItemsIdsT::NWKKEY:
            if (length == 0x15) {
                this->coordinator->memAligned = false;
            }
            else if (length == 0x18) {
                this->coordinator->memAligned = true;
            }
            break;
        case NvItemsIdsT::PRECFGKEY:
            if (length == LENGTH_NETWORK_KEY) {
                CopyToArray(data.at(2), this->coordinator->networkKey, LENGTH_NETWORK_KEY);
            }
            break;
        case NvItemsIdsT::NIB:
            if (length == 0x6E) {
                ClearMem(this->coordinator->nIB);
                CopyToStruct(data.at(2), this->coordinator->nIB, 15);
                this->coordinator->nIB.transactionPersistenceTime = BUILD_UINT16(data.at(17));
                this->coordinator->nIB.nwkProtocolVersion = data.at(19);
                this->coordinator->nIB.routeDiscoveryTime = data.at(20);
                this->coordinator->nIB.routeExpiryTime = data.at(21);
                this->coordinator->nIB.nwkDevAddr = BUILD_UINT16(data.at(22));
                this->coordinator->nIB.nwkLogicalChannel = data.at(24);
                this->coordinator->nIB.nwkCoordAddr = BUILD_UINT16(data.at(25));
                CopyToArray(data.at(27), this->coordinator->nIB.nwkCoordExtAddr, LENGTH_EXTADDR_IEEE);
                this->coordinator->nIB.nwkPanId = BUILD_UINT16(data.at(35));
                this->coordinator->nIB.nwkState = static_cast<NwkStatesT>(data.at(37));
                this->coordinator->nIB.channelList = static_cast<ZBChannelT>(BUILD_UINT32(data.at(38)));
                this->coordinator->nIB.beaconOrder = data.at(42);
                this->coordinator->nIB.superFrameOrder = data.at(43);
                this->coordinator->nIB.scanDuration = data.at(44);
                this->coordinator->nIB.battLifeExt = data.at(45);
                this->coordinator->nIB.allocatedRouterAddr = BUILD_UINT32(data.at(46));
                this->coordinator->nIB.allocatedEndDeviceAddr = BUILD_UINT32(data.at(50));
                this->coordinator->nIB.nodeDepth = data.at(54);
                CopyToArray(data.at(55), this->coordinator->nIB.extPanId, LENGTH_EXTADDR_IEEE);
                this->coordinator->nIB.nwkKeyLoaded = data.at(63);
                this->coordinator->nIB.spare1.keySeqNum = data.at(64);
                CopyToArray(data.at(65), this->coordinator->nIB.spare1.key, LENGTH_NETWORK_KEY);
                this->coordinator->nIB.spare2.keySeqNum = data.at(81);
                CopyToArray(data.at(82), this->coordinator->nIB.spare2.key, LENGTH_NETWORK_KEY);
                this->coordinator->nIB.spare3 = data.at(98);
                this->coordinator->nIB.spare4 = data.at(99);
                this->coordinator->nIB.nwkLinkStatusPeriod = data.at(100);
                this->coordinator->nIB.nwkRouterAgeLimit = data.at(101);
                this->coordinator->nIB.nwkUseMultiCast = data.at(102);
                this->coordinator->nIB.nwkIsConcentrator = data.at(103);
                this->coordinator->nIB.nwkConcentratorDiscoveryTime = data.at(104);
                this->coordinator->nIB.nwkConcentratorRadius = data.at(105);
                this->coordinator->nIB.nwkAllFresh = data.at(106);
                this->coordinator->nIB.nwkManagerAddr = BUILD_UINT16(data.at(107));
                this->coordinator->nIB.nwkTotalTransmissions = BUILD_UINT16(data.at(109));
                this->coordinator->nIB.nwkUpdateId = data.at(111);
            }
            else if (length == 0x74) {
                ClearMem(this->coordinator->nIB);
                CopyToStruct(data.at(2), this->coordinator->nIB, length);
            }
            break;
        default:
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processWriteOsalNVItems(vector<uint8_t>& data, void* value) {
    if (!data.size()) {
        return;
    }
    if (value != nullptr) {
        *static_cast<ZnpCommandStatusT*>(value) = static_cast<ZnpCommandStatusT>(data.at(0));
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processLengthOsalNVItems(vector<uint8_t>& data, void* value) {
    if (data.size() < 2) {
        return;
    }
    if (value != nullptr) {
        *static_cast<uint16_t*>(value) = BUILD_UINT16(data.at(0));
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processCoordVersion(vector<uint8_t>& data, void* value) {
    if (data.size() < 5) {
        return;
    }
    this->coordinator->transportRev = data.at(0);
    this->coordinator->product = static_cast<ZnpVersionT>(data.at(1));
    this->coordinator->majorRel = data.at(2);
    this->coordinator->minorRel = data.at(3);
    this->coordinator->maintRel = data.at(4);
    if (data.size() < 9) {
        return;
    }
    this->coordinator->revision = *reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(&data.at(5)));
    this->coordinator->supportsLed = (this->coordinator->product != zStack3x0 || (this->coordinator->product == zStack3x0 && this->coordinator->revision >= 20210430));
    this->coordinator->supportsAssocAdd = (this->coordinator->product == zStack3x0 && this->coordinator->revision >= 20201026);
    this->coordinator->supportsAssocRemove = (this->coordinator->product == zStack3x0 && this->coordinator->revision >= 20200805);
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processReadConfig(vector<uint8_t>& data, void* value) {
    if (!data.size()) {
        return;
    }
    if (data.at(0) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    NvItemsIdsT nvItem = static_cast<NvItemsIdsT>(data.at(1));
    uint8_t length = data.at(2);
    switch (this->coordinator->nvItem) {
        case NvItemsIdsT::PRECFGKEY:
            if (nvItem != NvItemsIdsT::PRECFGKEY) {
                break;
            }
            if (length == LENGTH_NETWORK_KEY) {
                CopyToArray(data.at(3), this->coordinator->networkKey, LENGTH_NETWORK_KEY);
            }
            break;
        default:
            break;
    }
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processDeviceInfo(vector<uint8_t>& data, void* value) {
    if (!data.size()) {
        return;
    }
    if (data.at(0) != ZnpCommandStatusT::SUCCESS_STATUS) {
        return;
    }
    uint16_t nwkAddr = BUILD_UINT16(data.at(9));
    switch (nwkAddr) {
        case NWK_ADDR_COORDINATOR:
        case NWK_ADDR_COORDINATOR_INIT:
            this->coordinator->address.addr.nwkAddr = nwkAddr;
            CopyToArray(data.at(1), this->coordinator->address.addr.ieeeAddr, LENGTH_EXTADDR_IEEE);
            this->coordinator->address.endpoint = EndpointListT::ENDPOINT1;
            this->coordinator->deviceType = data.at(11);
            this->coordinator->states = static_cast<DevStatesT>(data.at(12));
            this->coordinator->deviceCount = data.at(13);
            for (int i = 0; i < this->coordinator->deviceCount; ++i) {
                this->coordinator->deviceIdent[i].address.addr.nwkAddr = BUILD_UINT16(data.at(14 + 2*i));
            }
            break;
        default:
            break;
    }
}

#endif /* INC_ERA_FROM_STACK_ZIGBEE_HPP_ */
