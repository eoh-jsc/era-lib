#ifndef INC_ERA_TO_ZIGBEE_HPP_
#define INC_ERA_TO_ZIGBEE_HPP_

#include <stdint.h>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>
#include <Zigbee/ERaCommandZigbee.hpp>
#include "definition/ERaDefineZigbee.hpp"

template <class Zigbee>
class ERaToZigbee
    : public ERaCommandZigbee< ERaToZigbee<Zigbee> >
{
protected:
    enum ConvertToZigbeeT {
        CONVERT_SET_TO_ZIGBEE = 0x00,
        CONVERT_GET_TO_ZIGBEE = 0x01
    };

private:
    friend class ERaCommandZigbee< ERaToZigbee<Zigbee> >;

public:
    typedef ERaCommandZigbee< ERaToZigbee<Zigbee> > CommandZigbee;

    ERaToZigbee()
        : transId(0)
        , transIdZcl(0)
        , coordinator(InfoCoordinator_t::instance())
        , mutex(NULL)
    {}
    ~ERaToZigbee()
    {}

protected:
    bool toZigbee(const cJSON* const root, AFAddrType_t& dstAddr, const ConvertToZigbeeT type);
    bool permitJoinToZigbee(const cJSON* const root);

private:
    void handleZigbeeData() {
        this->thisZigbee().handleZigbeeData();
    }

    bool stateToZigbee(const cJSON* const root, const cJSON* const current, AFAddrType_t& dstAddr, const ConvertToZigbeeT type);
    bool levelToZigbee(const cJSON* const root, const cJSON* const current, AFAddrType_t& dstAddr, const ConvertToZigbeeT type);
    bool getEndpointToZigbee(const cJSON* const root, const char* key, EndpointListT& endpoint);
    bool findDeviceInfoWithIEEEAddr(AFAddrType_t& dstAddr);
    void createBridgeDataZigbee(const char* subTopic, ResultT status, const cJSON* const item);
    ResultT waitResponse(Response_t rspWait, void* value);
    ResultT createCommandBuffer(const vector<uint8_t>& payload,
                                TypeT type,
                                SubsystemT sub,
                                uint8_t cmd,
                                TypeT typeWait,
                                uint8_t cmdWait,
                                void* value = nullptr,
                                uint32_t timeout = MAX_TIMEOUT,
                                SubsystemT subWait = SubsystemT::RESERVED_INTER,
                                uint16_t nwkAddr = NO_NWK_ADDR,
                                ClusterIDT zclId = ClusterIDT::NO_CLUSTER_ID,
                                uint8_t* _transId = nullptr,
                                uint8_t* _transIdZcl = nullptr);
    ResultT createCommand(AFAddrType_t& dstAddr,
                                EndpointListT srcEndpoint,
                                ClusterIDT zclId,
                                const vector<uint8_t>& data,
                                uint8_t cmdWait,
                                uint8_t _transIdZcl,
                                void* value = nullptr,
                                uint32_t timeout = MAX_TIMEOUT);
    ResultT sendCommandInternalZigbee(HeaderZclFrame_t& zclHeader,
                                AFAddrType_t& dstAddr,
                                EndpointListT srcEndpoint,
                                CommandsZcl_t& cmd,
                                uint8_t cmdWait = AFCommandsT::AF_DATA_CONFIRM,
                                void* value = nullptr,
                                uint32_t timeout = DEFAULT_TIMEOUT,
                                void* _transId = nullptr,
                                bool force = false);
    ResultT sendCommandIdZigbee(HeaderZclFrame_t& zclHeader,
                                AFAddrType_t& dstAddr,
                                EndpointListT srcEndpoint,
                                CommandsZcl_t cmd,
                                uint8_t cmdWait = AFCommandsT::AF_DATA_CONFIRM,
                                void* value = nullptr,
                                uint32_t timeout = DEFAULT_TIMEOUT,
                                void* _transId = nullptr,
                                IdentDeviceAddr_t* deviceInfo = nullptr,
                                bool checkedNetAddr = false,
                                bool discoverRoute = false,
                                bool assocRemove = false,
                                bool assocRestore = false,
                                uint8_t nodeRelation = 0xFF,
                                uint8_t numRequest = 0x00,
                                bool store = true,
                                bool force = false);
    uint8_t createFrameControl(HeaderZclFrame_t& zclHeader);
    void createZclHeader(vector<uint8_t>& payload, HeaderZclFrame_t& zclHeader, uint8_t _transIdZcl, uint8_t cmdId);
    uint8_t getCheckSumCommand(const vector<uint8_t>& data);
    void sendByte(uint8_t byte);
    void sendCommand(const vector<uint8_t>& data);

    inline
    const Zigbee& thisZigbee() const {
        return static_cast<const Zigbee&>(*this);
    }

    inline
    Zigbee& thisZigbee() {
        return static_cast<Zigbee&>(*this);
    }

    uint8_t transId;
    uint8_t transIdZcl;

    InfoCoordinator_t*& coordinator;
    ERaMutex_t mutex;
};

template <class Zigbee>
bool ERaToZigbee<Zigbee>::toZigbee(const cJSON* const root, AFAddrType_t& dstAddr, const ConvertToZigbeeT type) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        if (type == ConvertToZigbeeT::CONVERT_GET_TO_ZIGBEE) {
            return false;
        }
    }

    cJSON* item = cJSON_GetObjectItem(root, "nwk_addr");
    if (cJSON_IsNumber(item)) {
        dstAddr.addr.nwkAddr = item->valueint;
    }

    if (!dstAddr.addr.nwkAddr) {
        if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
            return false;
        }
        if (!this->findDeviceInfoWithIEEEAddr(dstAddr)) {
            return false;
        }
        // Find the device with the given IEEE address.
    }

    cJSON* current = nullptr;

    for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
        if (this->stateToZigbee(root, current, dstAddr, type)) {
            continue;
        }
        if (this->levelToZigbee(root, current, dstAddr, type)) {
            continue;
        }
    }

    item = nullptr;
    current = nullptr;
    return true;
}

template <class Zigbee>
bool ERaToZigbee<Zigbee>::getEndpointToZigbee(const cJSON* const root, const char* key, EndpointListT& endpoint) {
    if (root == nullptr || key == nullptr) {
        return false;
    }
    bool onOff {false};
    char* ptr = strstr(root->string, key);
    if (strchr((ptr += strlen(key)), '_')) {
        if (CompareNString(ptr, "_onoff")) {
            onOff = true;
            ptr += strlen("_onoff");
            if (!strchr(ptr, '_')) {
                return onOff;
            }
        }
        std::string str(strchr(ptr, '_') + 1);
        if (!str.empty() && IsDigit(str)) {
            endpoint = static_cast<EndpointListT>(atoi(str.c_str()));
        }
    }
    return onOff;
}

template <class Zigbee>
bool ERaToZigbee<Zigbee>::findDeviceInfoWithIEEEAddr(AFAddrType_t& dstAddr) {
    if (IsZeroArray(dstAddr.addr.ieeeAddr)) {
        return false;
    }
    if (CompareArray(dstAddr.addr.ieeeAddr, this->coordinator->address.addr.ieeeAddr)) {
        return false;
    }
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent), find_deviceWithIEEEAddr_t(dstAddr.addr.ieeeAddr));
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        deviceInfo = &this->coordinator->deviceIdent[this->coordinator->deviceCount];
        CopyArray(dstAddr.addr.ieeeAddr, deviceInfo->address.addr.ieeeAddr);
        if (CommandZigbee::requestNwkAddrZstack(dstAddr, 0, 0) != ResultT::RESULT_SUCCESSFUL) {
            ClearMem(deviceInfo, sizeof(IdentDeviceAddr_t));
            return false;
        }
        if (!deviceInfo->address.addr.nwkAddr) {
            ClearMem(deviceInfo, sizeof(IdentDeviceAddr_t));
            return false;
        }
        /* Request model name */
        this->coordinator->deviceCount++;
        /* Store */
    }
    dstAddr.addrMode = AddressModeT::ADDR_16BIT;
    dstAddr.addr.nwkAddr = deviceInfo->address.addr.nwkAddr;

    return true;
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::createCommandBuffer(const vector<uint8_t>& payload,
                                                TypeT type,
                                                SubsystemT sub,
                                                uint8_t cmd,
                                                TypeT typeWait,
                                                uint8_t cmdWait,
                                                void* value,
                                                uint32_t timeout,
                                                SubsystemT subWait,
                                                uint16_t nwkAddr,
                                                ClusterIDT zclId,
                                                uint8_t* _transId,
                                                uint8_t* _transIdZcl) {
    vector<uint8_t> command;
    uint8_t fcs {0};
    ResultT status {ResultT::RESULT_SUCCESSFUL};
    command.push_back(this->thisZigbee().SOF);
    command.push_back(payload.size());
    command.push_back(((type << 5) & 0xE0) | (sub & 0x1F));
    command.push_back(cmd);
    command.insert(command.end(), payload.begin(), payload.end());
    fcs = getCheckSumCommand(command);
    command.push_back(fcs);

    this->sendCommand(command);
    status = this->waitResponse({nwkAddr, typeWait, (subWait == SubsystemT::RESERVED_INTER) ? sub : subWait,
                                cmdWait, zclId, static_cast<uint8_t>(_transId != nullptr ? *_transId : 0x00),
                                static_cast<uint8_t>(_transIdZcl != nullptr ? *_transIdZcl : 0x00), ZnpCommandStatusT::INVALID_PARAM, timeout}, value);

    ERA_ZIGBEE_YIELD();
    return status;
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::createCommand(AFAddrType_t& dstAddr,
                                        EndpointListT srcEndpoint,
                                        ClusterIDT zclId,
                                        const vector<uint8_t>& data,
                                        uint8_t cmdWait,
                                        uint8_t _transIdZcl,
                                        void* value,
                                        uint32_t timeout) {
    if (dstAddr.addrMode != AddressModeT::ADDR_16BIT) {
        return ResultT::RESULT_FAIL;
    }
    if (!dstAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }
    if (dstAddr.endpoint == EndpointListT::ENDPOINT_NONE) {
        dstAddr.endpoint = EndpointListT::ENDPOINT1;
    }
    if (srcEndpoint == EndpointListT::ENDPOINT_NONE) {
        srcEndpoint = EndpointListT::ENDPOINT1;
    }

    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(dstAddr.endpoint);
    payload.push_back(srcEndpoint);
    payload.push_back(LO_UINT16(zclId));
    payload.push_back(HI_UINT16(zclId));
    payload.push_back(++this->transId);
    payload.push_back(this->thisZigbee().Options);
    payload.push_back(this->thisZigbee().Radius);
    payload.push_back(data.size());
    payload.insert(payload.end(), data.begin(), data.end());

    if (cmdWait == AFCommandsT::AF_INCOMING_MSG) {
        return this->createCommandBuffer(payload, TypeT::SREQ, SubsystemT::AF_INTER, AFCommandsT::AF_DATA_REQUEST,
                                        TypeT::AREQ, cmdWait, value, timeout,
                                        SubsystemT::RESERVED_INTER, dstAddr.addr.nwkAddr, zclId, &this->transId, &_transIdZcl);
    }
    else {
        return this->createCommandBuffer(payload, TypeT::SREQ, SubsystemT::AF_INTER, AFCommandsT::AF_DATA_REQUEST,
                                        TypeT::AREQ, cmdWait, value, timeout,
                                        SubsystemT::RESERVED_INTER, NO_NWK_ADDR, ClusterIDT::NO_CLUSTER_ID, &this->transId, &_transIdZcl);
    }
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::sendCommandInternalZigbee(HeaderZclFrame_t& zclHeader,
                                                    AFAddrType_t& dstAddr,
                                                    EndpointListT srcEndpoint,
                                                    CommandsZcl_t& cmd,
                                                    uint8_t cmdWait,
                                                    void* value,
                                                    uint32_t timeout,
                                                    void* _transId,
                                                    bool force) {
    if (dstAddr.addrMode != AddressModeT::ADDR_16BIT) {
        return ResultT::RESULT_FAIL;
    }
    if (!dstAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }

    vector<uint8_t> payload;

    this->createZclHeader(payload, zclHeader, (_transId != nullptr ? *static_cast<uint8_t*>(_transId) : ++this->transIdZcl), cmd.command);
    
    if(cmd.data != nullptr) {
        payload.insert(payload.end(), cmd.data->begin(), cmd.data->end());
    }

    return this->createCommand(dstAddr, srcEndpoint, cmd.zclId, payload,
                            cmdWait, (_transId != nullptr ? *static_cast<uint8_t*>(_transId) : this->transIdZcl), value, timeout);
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::sendCommandIdZigbee(HeaderZclFrame_t& zclHeader,
                                                AFAddrType_t& dstAddr,
                                                EndpointListT srcEndpoint,
                                                CommandsZcl_t cmd,
                                                uint8_t cmdWait,
                                                void* value,
                                                uint32_t timeout,
                                                void* _transId,
                                                IdentDeviceAddr_t* deviceInfo,
                                                bool checkedNetAddr,
                                                bool discoverRoute,
                                                bool assocRemove,
                                                bool assocRestore,
                                                uint8_t nodeRelation,
                                                uint8_t numRequest,
                                                bool store,
                                                bool force) {
    if (dstAddr.addrMode != AddressModeT::ADDR_16BIT) {
        return ResultT::RESULT_FAIL;
    }
    if (!dstAddr.addr.nwkAddr) {
        return ResultT::RESULT_FAIL;
    }

    ResultT status {ResultT::RESULT_SUCCESSFUL};

    status = this->sendCommandInternalZigbee(zclHeader, dstAddr, srcEndpoint, cmd,
                                            cmdWait, value, timeout, _transId, force);

    return status;
}

template <class Zigbee>
uint8_t ERaToZigbee<Zigbee>::createFrameControl(HeaderZclFrame_t& zclHeader) {
    bool manufSpec = (zclHeader.manufCode != ManufacturerCodesT::MANUF_CODE_NONE ? true : false);
    uint8_t frameCtrl = zclHeader.frameType & 0x03;
    frameCtrl |= (manufSpec << 2) & 0x04;
    frameCtrl |= (zclHeader.direction << 3) & 0x08;
    frameCtrl |= (zclHeader.disableRsp << 4) & 0x10;
    frameCtrl |= (zclHeader.reservedBits << 5) & 0xE0;
    return frameCtrl;
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::createZclHeader(vector<uint8_t>& payload, HeaderZclFrame_t& zclHeader, uint8_t _transIdZcl, uint8_t cmdId) {
    payload.push_back(this->createFrameControl(zclHeader));
    if(zclHeader.manufCode != ManufacturerCodesT::MANUF_CODE_NONE) {
        payload.push_back(LO_UINT16(zclHeader.manufCode));
        payload.push_back(HI_UINT16(zclHeader.manufCode));
    }
    payload.push_back(_transIdZcl);
    payload.push_back(cmdId);
}

template <class Zigbee>
uint8_t ERaToZigbee<Zigbee>::getCheckSumCommand(const vector<uint8_t>& data) {
    uint8_t crc {0};
    for (auto i = data.begin() + 1; i != data.end(); ++i) {
        crc = crc ^ *i;
    }
    return crc;
}

#include "toZigbee/ERaToBridge.hpp"
#include "toZigbee/ERaToOnOff.hpp"
#include "toZigbee/ERaToLevel.hpp"

#endif /* INC_ERA_TO_ZIGBEE_HPP_ */
