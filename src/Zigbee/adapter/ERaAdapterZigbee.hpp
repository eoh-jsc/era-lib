#ifndef INC_ERA_ADAPTER_ZIGBEE_HPP_
#define INC_ERA_ADAPTER_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

template <class Api>
void ERaZigbee<Api>::startZigbee(bool& format, bool& invalid) {
    this->coordinator->reset();
    this->coordinator->lock = true;
    uint8_t index {0};
    size_t numEpList {0};

    if (ToZigbee::CommandZigbee::pingSystem(10) != ResultT::RESULT_SUCCESSFUL) {
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FAIL);
        return;
    }
    ToZigbee::CommandZigbee::versionSystem(10);
    ToZigbee::CommandZigbee::nwkKeyReadOSALZstack(10);
    ToZigbee::CommandZigbee::znpHasConfiguredReadOSALZstack(10);
    ToZigbee::CommandZigbee::nibReadOSALZstack(10);
    ToZigbee::CommandZigbee::channelListReadOSALZstack(10);
    ToZigbee::CommandZigbee::panIdReadOSALZstack(10);
    ToZigbee::CommandZigbee::getExtAddrSystem(1);

    if (!CheckInfoCoordinator_t(this->coordinator)) {
        format = true;
    }

    if (format) {
        // parse network option update soon
        this->coordinator->address.panId = ERaRandomNumber(0x1000, 0xF000);
        if (this->coordinator->address.panId < 0x1000 || this->coordinator->address.panId > 0xF000) {
            this->coordinator->address.panId = this->DefaultPanId;
        }
        if (invalid || (this->coordinator->hasConfigured != FLAG_ZIGBEE_HAS_CONFIGURED) ||
                        (this->coordinator->channel == ZBChannelT::CHANNEL_NONE) ||
                        (this->coordinator->channel == ZBChannelT::NO_LOAD_CHANNEL)) {
            index = ERaRandomNumber(0, 15);
            if (index > 15) {
                this->coordinator->channel = this->DefaultChannel;
            }
            else {
                this->coordinator->channel = this->ChannelList[index];
            }
        }
        ToZigbee::CommandZigbee::getDeviceInfoZstack(10);
        if (IsZeroArray(this->coordinator->address.addr.ieeeAddr)) {
            CopyArray(this->coordinator->extAddr, this->coordinator->address.addr.ieeeAddr);
        }
        if (!IsZeroArray(this->coordinator->address.addr.ieeeAddr)) {
            CopyArray(this->coordinator->address.addr.ieeeAddr, this->coordinator->extPanId);
        }

        // Network key (update soon...)
        this->generateNetworkKey(this->coordinator->networkKey);
        CopyArray(this->DefaultNetworkKey, this->coordinator->networkKey);

        ToZigbee::CommandZigbee::requestResetZstack(ResetTypeT::RST_SOFT, 1);
        ToZigbee::CommandZigbee::deleteItemZstack(NvItemsIdsT::NIB);
        ToZigbee::CommandZigbee::startupOptionWriteOSALZstack(0x03);
        ToZigbee::CommandZigbee::requestResetZstack(ResetTypeT::RST_SOFT, 1);
        ToZigbee::CommandZigbee::startupOptionWriteOSALZstack(0x00);
        ToZigbee::CommandZigbee::logicalTypeWriteOSALZstack(TypeDeviceT::COORDINATOR);
        ToZigbee::CommandZigbee::zdoDirectWriteOSALZstack(true);
        ToZigbee::CommandZigbee::channelListWriteOSALZstack(this->coordinator->channel);
        ToZigbee::CommandZigbee::panIdWriteOSALZstack(this->coordinator->address.panId);
        ToZigbee::CommandZigbee::extendedPanIdWriteOSALZstack(this->coordinator->extPanId);
        ToZigbee::CommandZigbee::apsExtPanIdWriteOSALZstack(this->coordinator->extPanId);
        ToZigbee::CommandZigbee::enablePrecfgkeysWriteOSALZstack(false);
        ToZigbee::CommandZigbee::precfgkeysWriteZstack(this->coordinator->networkKey);
        ToZigbee::CommandZigbee::touchlinkKeyWriteOSALZstack(this->TcLinkKey);

        if (this->coordinator->product != ZnpVersionT::zStack12) {
            ToZigbee::CommandZigbee::setBDBChannelZstack(this->coordinator->channel, true);
            ToZigbee::CommandZigbee::setBDBChannelZstack(ZBChannelT::CHANNEL_NONE, false);
            if (ToZigbee::CommandZigbee::stateStartupAppZstack(10) != ResultT::RESULT_SUCCESSFUL) {
                invalid = format;
                ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FAIL);
                return;
            }
            ToZigbee::CommandZigbee::netFormationStartCommZstack();
        }

        ToZigbee::CommandZigbee::znpHasConfiguredWriteOSALInitZstack();
        ToZigbee::CommandZigbee::znpHasConfiguredWriteOSALZstack();

        // store network option
    }
    else {
        ToZigbee::CommandZigbee::enablePrecfgkeysReadOSALZstack(2);
        ToZigbee::CommandZigbee::precfgkeysReadZstack(2);
        ToZigbee::CommandZigbee::panIdReadOSALZstack(2);
        ToZigbee::CommandZigbee::extendedPanIdReadOSALZstack(2);
    }

    ToZigbee::CommandZigbee::getDeviceInfoZstack(10);
    ERaDelay(3000);

    if (ToZigbee::CommandZigbee::stateStartupAppZstack(10) != ResultT::RESULT_SUCCESSFUL) {
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FAIL);
        return;
    }

    if (ToZigbee::CommandZigbee::requestListEndpointZstack(0, 0, 10) != ResultT::RESULT_SUCCESSFUL) {
        invalid = format;
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FAIL);
        return;
    }

    numEpList = ERA_COUNT_OF(EpList);

    while (coordinator->epCount < numEpList) {
        for (size_t i = 0; i < numEpList; ++i) {
            ToZigbee::CommandZigbee::afRegisterEndpointZstack(EpList[i]);
        }
        ToZigbee::CommandZigbee::requestListEndpointZstack(0, 0, 10);
    }

    ToZigbee::CommandZigbee::getDeviceInfoZstack(10);
    ToZigbee::CommandZigbee::requestSimpleDescZstack(EpList);
    ToZigbee::CommandZigbee::requestIEEEAddrZstack(this->coordinator->address, 0, 0);

    ToZigbee::CommandZigbee::setTuningOperationZstack(OperationModeT::SET_TRANSMIT_POWER, this->coordinator->transmitPower);

    ToZigbee::CommandZigbee::versionSystem(1);
    ToZigbee::CommandZigbee::extAddrReadOSALZstack(1);
    if (ToZigbee::CommandZigbee::waitNibReadOSALZstack(10) != ResultT::RESULT_SUCCESSFUL) {
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FAIL);
        return;
    }
    ToZigbee::CommandZigbee::panIdReadOSALZstack(1);
    ToZigbee::CommandZigbee::extendedPanIdReadOSALZstack(1);
    ToZigbee::CommandZigbee::activeKeyReadOSALZstack(1);
    ToZigbee::CommandZigbee::alternKeyReadOSALZstack(1);
    ToZigbee::CommandZigbee::apsExtPanIdReadOSALZstack(1);
    ToZigbee::CommandZigbee::apsLinkKeyReadOSALZstack(1);
    ToZigbee::CommandZigbee::precfgkeysReadZstack(1);
    ToZigbee::CommandZigbee::enablePrecfgkeysReadOSALZstack(1);
    ToZigbee::CommandZigbee::channelListReadOSALZstack(1);
    ToZigbee::CommandZigbee::touchlinkKeyReadOSALZstack(1);
    ToZigbee::CommandZigbee::materialTableReadOSALZstack(1);

    ToZigbee::CommandZigbee::requestInfoNwkExtZstack(1);

    this->permitJoinDuration(this->coordinator->permitJoin.address, 0x00);

    this->coordinator->clearAllDevice();
    DBZigbee::parseZigbeeDevice();

    this->createInfoCoordinator();

    ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_SUCCESSFUL);
}

template <class Api>
ResultT ERaZigbee<Api>::permitJoinDuration(AFAddrType_t& dstAddr, uint8_t seconds) {
    ResultT status {ResultT::RESULT_SUCCESSFUL};
    this->coordinator->permitJoin.address = dstAddr;
    switch (seconds) {
        case 0x00:
            this->coordinator->permitJoin.address = PermitJoin_t().address;
            if (this->coordinator->permitJoin.installCode) {
                this->coordinator->permitJoin.installCode = false;
            }
            /* Commissioning Green Power */
            status = ToZigbee::CommandZigbee::permitJoinRequest(dstAddr, seconds);
            this->coordinator->permitJoin.enable = false;
            if (this->timerJoin) {
                this->timerJoin.deleteTimer();
            }
            ZigbeeState::set(ZigbeeStateT::STATE_ZB_RUNNING);
            break;
        default:
            status = ToZigbee::CommandZigbee::permitJoinRequest(dstAddr, seconds);
            /* Commissioning Green Power */
            this->coordinator->permitJoin.enable = true;
            if (!this->timerJoin) {
                this->timerJoin = this->timer.setInterval(PERMIT_JOIN_INTERVAL, [=](void* args) {
                    this->zigbeeTimerCallback(args);
                }, &this->timerJoin);
            }
            ZigbeeState::set(ZigbeeStateT::STATE_ZB_PERMIT_JOIN);
            break;
    }
    return status;
}

template <class Api>
void ERaZigbee<Api>::factoryResetZigbee() {
    /* Format setting */
    ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_FORMAT);
}

template <class Api>
void ERaZigbee<Api>::pingCoordinator() {
    if (ToZigbee::CommandZigbee::pingSystem(10) == ResultT::RESULT_SUCCESSFUL) {
        return;
    }
    ERA_LOG(TAG, ERA_PSTR("Ping coordinator failed, resetting coordinator!"));
    ZigbeeState::set(ZigbeeStateT::STATE_ZB_INIT_MAX);
}

template <class Api>
template <int size>
void ERaZigbee<Api>::generateNetworkKey(uint8_t(&nwkKey)[size]) {
    if ((size != LENGTH_NETWORK_KEY) ||
        IsZeroArray(this->coordinator->address.addr.ieeeAddr)) {
        return CopyArray(this->DefaultNetworkKey, nwkKey);
    }

    size_t index {0};
    size_t len = sizeof(this->DefaultNetworkKey) / sizeof(this->DefaultNetworkKey[0]);
    for (size_t i = 0; i < len; ++i) {
        nwkKey[i] = this->coordinator->address.addr.ieeeAddr[index++ % LENGTH_EXTADDR_IEEE] ^ this->DefaultNetworkKey[i];
    }
}

template <class Api>
void ERaZigbee<Api>::createInfoCoordinator() {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON* netItem = cJSON_CreateObject();
    if (netItem == nullptr) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    cJSON_AddStringToObject(root, "type", ((this->coordinator->product == ZnpVersionT::zStack12) ? "Zigbee 1.2 HA" : "Zigbee 3.0"));
    cJSON_AddNumberToObject(netItem, "channel", this->coordinator->nIB.nwkLogicalChannel);
    cJSON_AddNumberToObject(netItem, "nwk_addr", this->coordinator->address.addr.nwkAddr);
    cJSON_AddStringToObject(netItem, "ieee_addr", IEEEToString(this->coordinator->address.addr.ieeeAddr).c_str());
    cJSON_AddNumberToObject(netItem, "pan_id", this->coordinator->address.panId);
    cJSON_AddStringToObject(netItem, "ext_pan_id", IEEEToString(this->coordinator->extPanId).c_str());

    switch (this->coordinator->commStatus) {
        case APPCFCommStatusT::BDB_COMMISSIONING_SUCCESS:
            cJSON_AddStringToObject(netItem, "commissioning", "success");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_IN_PROGRESS:
            cJSON_AddStringToObject(netItem, "commissioning", "in progress");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_NO_NETWORK:
            cJSON_AddStringToObject(netItem, "commissioning", "no network");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_TL_TARGET_FAILURE:
            cJSON_AddStringToObject(netItem, "commissioning", "target failure");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_TL_NOT_AA_CAPABLE:
            cJSON_AddStringToObject(netItem, "commissioning", "tl not aa capable");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_TL_NO_SCAN_RESPONSE:
            cJSON_AddStringToObject(netItem, "commissioning", "tl no scan response");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_TL_NOT_PERMITTED:
            cJSON_AddStringToObject(netItem, "commissioning", "tl not permitted");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_TCLK_EX_FAILURE:
            cJSON_AddStringToObject(netItem, "commissioning", "tclk ex failure");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FORMATION_FAILURE:
            cJSON_AddStringToObject(netItem, "commissioning", "formation failure");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FB_TARGET_IN_PROGRESS:
            cJSON_AddStringToObject(netItem, "commissioning", "fb target in progress");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FB_INITIATOR_IN_PROGRESS:
            cJSON_AddStringToObject(netItem, "commissioning", "fb initiator in progress");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FB_NO_IDENTIFY_QUERY_RESPONSE:
            cJSON_AddStringToObject(netItem, "commissioning", "fb no identify query response");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FB_BINDING_TABLE_FULL:
            cJSON_AddStringToObject(netItem, "commissioning", "fb binding table full");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_NETWORK_RESTORED:
            cJSON_AddStringToObject(netItem, "commissioning", "network restored");
            break;
        case APPCFCommStatusT::BDB_COMMISSIONING_FAILURE:
            cJSON_AddStringToObject(netItem, "commissioning", "failure");
            break;
        default:
            cJSON_AddNumberToObject(netItem, "commissioning", this->coordinator->commStatus);
            break;
    }

    cJSON_AddItemToObject(root, "network", netItem);
    cJSON_AddNumberToObject(root, "transmit_power", this->coordinator->transmitPower);
    cJSON_AddBoolToObject(root, "permit_join", this->coordinator->permitJoin.enable);
    cJSON_AddNumberToObject(root, "device_count", this->coordinator->deviceCount);

    this->publishZigbeeData(TOPIC_ZIGBEE_BRIDGE_INFO, root);

    cJSON_Delete(root);
    root = nullptr;
    netItem = nullptr;
}

template <class Api>
void ERaZigbee<Api>::zigbeeTimerCallback(void* args) {
    if (args == nullptr) {
        return;
    }
    ERaTimer::iterator* tm = (ERaTimer::iterator*)args;
    if (tm->getId() == this->timerJoin.getId()) {
        this->permitJoinDuration(this->coordinator->permitJoin.address, 254);
    }
    else if (tm->getId() == this->timerPing.getId()) {
        this->pingCoordinator();
    }
    else if (tm->getId() == this->timerJoined.getId()) {
        if (ZigbeeState::is(ZigbeeStateT::STATE_ZB_DEVICE_JOINED)) {
            ZigbeeState::set(ZigbeeStateT::STATE_ZB_PERMIT_JOIN);
        }
    }
}

#endif /* INC_ERA_ADAPTER_ZIGBEE_HPP_ */
