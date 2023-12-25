#ifndef INC_ERA_FROM_ZIGBEE_HPP_
#define INC_ERA_FROM_ZIGBEE_HPP_

#include <stdint.h>
#include <Utility/ERacJSON.hpp>
#include "definition/ERaDefineZigbee.hpp"

template <class Zigbee>
class ERaFromZigbee
{
public:
    ERaFromZigbee()
        : device(InfoDevice_t::instance())
        , coordinator(InfoCoordinator_t::instance())
    {}
    ~ERaFromZigbee()
    {}

protected:
    Response_t fromZigbee(const uint8_t* payload, void* value = nullptr);
    void createDeviceEvent(const DeviceEventT event, const AFAddrType_t* dstAddr = nullptr);

private:
    bool powerConfigFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool onOffFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool onOffSpecificFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, DefaultRsp_t& defaultRsp);
    bool levelFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool levelSpecificFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, DefaultRsp_t& defaultRsp);
    bool multistateInputFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool temperatureMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool pressureMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    bool humidityMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value);
    void processNodeDescriptor(vector<uint8_t>& data, void* value = nullptr);
    void processSimpleDescriptor(vector<uint8_t>& data, void* value = nullptr);
    void processActiveEndpoint(vector<uint8_t>& data, void* value = nullptr);
    void processBindUnbind(vector<uint8_t>& data, void* value = nullptr);
    void processTCDeviceIndication(vector<uint8_t>& data, void* value = nullptr);
    void processZDOState(vector<uint8_t>& data, void* value = nullptr);
    void processDeviceAnnounce(vector<uint8_t>& data, void* value = nullptr);
    void processDeviceLeave(vector<uint8_t>& data, void* value = nullptr);
    void processReadOsalNVItems(vector<uint8_t>& data, void* value = nullptr);
    void processWriteOsalNVItems(vector<uint8_t>& data, void* value = nullptr);
    void processLengthOsalNVItems(vector<uint8_t>& data, void* value = nullptr);
    void processCoordVersion(vector<uint8_t>& data, void* value = nullptr);
    void processReadConfig(vector<uint8_t>& data, void* value = nullptr);
    void processDeviceInfo(vector<uint8_t>& data, void* value = nullptr);
    bool getDataAFMsg(DataAFMsg_t& afMsg, vector<uint8_t>& data);
    void processDataAFMsg(const DataAFMsg_t& afMsg, Response_t& rsp, void* value = nullptr);
    void processFrameTypeGlobal(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    void processFrameTypeSpecific(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    void processReadAttribute(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    void processReadAttributeNormal(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp);
    void processReadAttributeResponse(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    void processReportAttribute(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    void processDefaultResponse(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value = nullptr);
    cJSON* createDeviceEndpoints();
    cJSON* createDevicePollControl();
    bool getDataAttributes(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, uint16_t nwkAddr, EndpointListT endpoint, vector<DataAttr_t> listAttr);
    bool getDataAttributesRsp(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, vector<DataReadAttrRsp_t> listData);
    uint8_t getCheckSumReceive(const uint8_t* pData, size_t pDataLen);

    IdentDeviceAddr_t* createDataGlobal(const DataAFMsg_t& afMsg, uint16_t attribute, uint8_t type, uint64_t& value);
    IdentDeviceAddr_t* createDataSpecific(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp);

    template <typename T>
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, T value);

    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, float value);
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, double value);
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, char* value);
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, const char* value);
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, cJSON* value);
    void addDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, const cJSON* value);
    cJSON* getDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key);
    void removeDataZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key);

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
Response_t ERaFromZigbee<Zigbee>::fromZigbee(const uint8_t* payload, void* value) {
    DataAFMsg_t afMsg {0};
    vector<uint8_t> data;
    Response_t rsp {
        .nwkAddr = NO_NWK_ADDR,
        .type = TypeT::ERR,
        .subSystem = 0,
        .command = 0,
        .zclId = ClusterIDT::NO_CLUSTER_ID,
        .transId = 0,
        .transIdZcl = 0,
        .cmdStatus = ZnpCommandStatusT::INVALID_PARAM,
        .timeout = 0,
    };
    uint8_t fcs {0};
    if (payload[this->thisZigbee().PositionSOF] != this->thisZigbee().SOF) {
        return rsp;
    }
    uint8_t length = payload[this->thisZigbee().PositionDataLength];
    uint8_t type = (payload[this->thisZigbee().PositionCmd0] & 0xE0) >> 5;
    uint8_t sub = payload[this->thisZigbee().PositionCmd0] & 0x1F;
    uint8_t cmd = payload[this->thisZigbee().PositionCmd1];
    data.assign(payload + this->thisZigbee().DataStart, payload + this->thisZigbee().DataStart + length);
    if (data.size() > this->thisZigbee().MaxDataSize) {
        return rsp;
    }
    fcs = this->getCheckSumReceive(payload + this->thisZigbee().PositionDataLength, length + 3);
    if(fcs != payload[length + 4]) {
        return rsp;
    }

    rsp = {
        .nwkAddr = NO_NWK_ADDR,
        .type = type,
        .subSystem = sub,
        .command = cmd,
        .zclId = ClusterIDT::NO_CLUSTER_ID,
        .transId = 0,
        .transIdZcl = 0,
        .cmdStatus = ZnpCommandStatusT::INVALID_PARAM,
        .timeout = 0
    };

    if (type == TypeT::AREQ) {
        switch (sub) {
            case SubsystemT::SYS_INTER:
                break;
            case SubsystemT::AF_INTER:
                if (cmd == AFCommandsT::AF_INCOMING_MSG) {
                    if (this->getDataAFMsg(afMsg, data)) {
                        this->processDataAFMsg(afMsg, rsp, value);
                        rsp.zclId = afMsg.zclId;
                        rsp.nwkAddr = afMsg.srcAddr.addr.nwkAddr;
                    }
                }
                else if (cmd == AFCommandsT::AF_INCOMING_MSG_EXT) {

                }
                else if (cmd == AFCommandsT::AF_DATA_CONFIRM) {
                    rsp.cmdStatus = data.at(0);
                    rsp.transId = data.at(2);
                }
                break;
            case SubsystemT::ZDO_INTER:
                if (cmd == ZDOCommandsT::ZDO_NODE_DESC_RSP) {
                    this->processNodeDescriptor(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_SIMPLE_DESC_RSP) {
                    this->processSimpleDescriptor(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_ACTIVE_EP_RSP) {
                    this->processActiveEndpoint(data, value);
                }
                else if ((cmd == ZDOCommandsT::ZDO_BIND_RSP) ||
                        (cmd == ZDOCommandsT::ZDO_UNBIND_RSP)) {
                    this->processBindUnbind(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_TC_DEV_IND) {
                    this->processTCDeviceIndication(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_STATE_CHANGE_IND) {
                    this->processZDOState(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_END_DEVICE_ANNCE_IND) {
                    this->processDeviceAnnounce(data, value);
                }
                else if (cmd == ZDOCommandsT::ZDO_LEAVE_IND) {
                    this->processDeviceLeave(data, value);
                }
                break;
            case SubsystemT::APP_CNF_INTER:
                break;
            default:
                break;
        }
    }
    else if (type == TypeT::SRSP) {
        switch (sub) {
            case SubsystemT::SYS_INTER:
                if (cmd == SYSCommandsT::SYS_OSAL_NV_READ ||
                    cmd == SYSCommandsT::SYS_OSAL_NV_READ_EXT) {
                    this->processReadOsalNVItems(data, value);
                }
                else if (cmd == SYSCommandsT::SYS_OSAL_NV_WRITE ||
                        cmd == SYSCommandsT::SYS_OSAL_NV_WRITE_EXT ||
                        cmd == SYSCommandsT::SYS_NV_WRITE) {
                    this->processWriteOsalNVItems(data, value);
                }
                else if (cmd == SYSCommandsT::SYS_OSAL_NV_LENGTH) {
                    this->processLengthOsalNVItems(data, value);
                }
                else if (cmd == SYSCommandsT::SYS_NV_LENGTH) {
                }
                else if (cmd == SYSCommandsT::SYS_NV_READ) {
                }
                else if (cmd == SYSCommandsT::SYS_VERSION) {
                    this->processCoordVersion(data, value);
                }
                break;
            case SubsystemT::ZDO_INTER:
                if (cmd == ZDOCommandsT::ZDO_EXT_FIND_GROUP) {
                }
                else if (cmd == ZDOCommandsT::ZDO_EXT_NWK_INFO) {
                }
                break;
            case SubsystemT::SAPI_INTER:
                if (cmd == ZBCommandsT::ZB_READ_CONFIGURATION) {
                    this->processReadConfig(data, value);
                }
                break;
            case SubsystemT::UTIL_INTER:
                if (cmd == UTILCommandsT::UTIL_GET_DEVICE_INFO) {
                    this->processDeviceInfo(data, value);
                }
                else if (cmd == UTILCommandsT::UTIL_ASSOC_GET_WITH_ADDRESS) {
                }
                break;
            default:
                break;
        }
    }

    return rsp;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::getDataAFMsg(DataAFMsg_t& afMsg, vector<uint8_t>& data) {
    afMsg.groupId = BUILD_UINT16(data.at(0));
    afMsg.zclId = static_cast<ClusterIDT>(BUILD_UINT16(data.at(2)));
    afMsg.srcAddr.addrMode = AddressModeT::ADDR_16BIT;
    afMsg.srcAddr.addr.nwkAddr = BUILD_UINT16(data.at(4));
    afMsg.srcAddr.endpoint = static_cast<EndpointListT>(data.at(6));
    afMsg.dstEndpoint = static_cast<EndpointListT>(data.at(7));
    afMsg.wasBroadcast = data.at(8);
    afMsg.linkQuality = data.at(9);
    afMsg.securityUse = data.at(10);
    afMsg.timestamp = BUILD_UINT32(data.at(11));
    afMsg.seqNum = data.at(15);
    afMsg.len = data.at(16);
    if (afMsg.len < ZCL_DATA_MIN) {
        return false;
    }
    afMsg.frameCtrl = data.at(17);
    bool manufSpec = (afMsg.frameCtrl >> 2) & 0x01;
    uint8_t manufShift = (manufSpec ? 2 : 0);
    if (afMsg.len < ZCL_DATA_MIN + manufShift) {
        return false;
    }
    if (manufSpec) {
        afMsg.manufCode = static_cast<ManufacturerCodesT>(BUILD_UINT16(data.at(18)));
    }
    else {
        afMsg.manufCode = ManufacturerCodesT::MANUF_CODE_NONE;
    }
    afMsg.receiveId = data.at(18 + manufShift);
    afMsg.cmdId = data.at(19 + manufShift);
    afMsg.pDataLen = afMsg.len - ZCL_DATA_MIN - manufShift;
    if (afMsg.pDataLen) {
        afMsg.pData = &data.at(20 + manufShift);
    }
    afMsg.radius = data.at(19 + afMsg.len); /* Radius (remain) - limits the number of hops */
    if ((afMsg.radius < this->thisZigbee().Radius - 1) || (afMsg.zclId == ClusterIDT::ZCL_CLUSTER_GREEN_POWER)) { /* afMsg.srcEndpoint == ENDPOINT242 */
        afMsg.parentAddr = BUILD_UINT16(data.at(17 + afMsg.len)); /* MAC header source short address */
    }
    if (afMsg.parentAddr == afMsg.srcAddr.addr.nwkAddr) {
        afMsg.parentAddr = 0;
    }
    afMsg.ddr = ((afMsg.frameCtrl >> 4) & 0x01);
    return true;
}

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processDataAFMsg(const DataAFMsg_t& afMsg, Response_t& rsp, void* value) {
    FrameTypeT type = static_cast<FrameTypeT>(afMsg.frameCtrl & 0x03);
    bool manufSpec = (afMsg.frameCtrl >> 2) & 0x01;
    DirectionT direction = static_cast<DirectionT>((afMsg.frameCtrl >> 3) & 0x01);
    bool disableRsp = (afMsg.frameCtrl >> 4) & 0x01;
    uint8_t reservedBits = afMsg.frameCtrl >> 5;

    uint8_t manufShift = (manufSpec ? 2 : 0);

    if (afMsg.len < ZCL_DATA_MIN + manufShift) {
        return;
    }

    DefaultRsp_t defaultRsp {};
    defaultRsp.isSpecific = ((type == FrameTypeT::SPECIFIC) ? true : false);
    defaultRsp.frameCtrl = afMsg.frameCtrl;
    defaultRsp.manufCode = afMsg.manufCode;
    defaultRsp.zclId = afMsg.zclId;
    defaultRsp.dstEndpoint = afMsg.srcAddr.endpoint;
    defaultRsp.srcEndpoint = afMsg.dstEndpoint;
    defaultRsp.dstAddr = afMsg.srcAddr.addr.nwkAddr;
    defaultRsp.transId = afMsg.receiveId;
    defaultRsp.hasResponse = afMsg.wasBroadcast; /* skip default response if broadcast */
    rsp.transIdZcl = afMsg.receiveId;

    if (type == FrameTypeT::GLOBAL) {
        this->processFrameTypeGlobal(afMsg, defaultRsp, value);
    }
    else {
        this->processFrameTypeSpecific(afMsg, defaultRsp, value);
    }

    if (!defaultRsp.isSent) {
        /* Queue Zigbee Rsp */
        if (this->thisZigbee().queueDefaultRsp.writeable()) {
            this->thisZigbee().queueDefaultRsp += defaultRsp;
        }
    }

    ERA_FORCE_UNUSED(manufSpec);
    ERA_FORCE_UNUSED(direction);
    ERA_FORCE_UNUSED(disableRsp);
    ERA_FORCE_UNUSED(reservedBits);
}

template <class Zigbee>
uint8_t ERaFromZigbee<Zigbee>::getCheckSumReceive(const uint8_t* pData, size_t pDataLen) {
    uint8_t crc {0};
    for (size_t i = 0; i < pDataLen; ++i) {
        crc = crc ^ pData[i];
    }
    return crc;
}

#include "fromZigbee/ERaFromKey.hpp"
#include "adapter/ERaClusterZigbee.hpp"
#include "fromZigbee/ERaFromScale.hpp"
#include "fromZigbee/ERaFromGlobal.hpp"
#include "fromZigbee/ERaFromSpecific.hpp"
#include "fromZigbee/ERaFromData.hpp"
#include "fromZigbee/ERaFromPowerConfig.hpp"
#include "fromZigbee/ERaFromOnOff.hpp"
#include "fromZigbee/ERaFromLevel.hpp"
#include "fromZigbee/ERaFromMultistateInput.hpp"
#include "fromZigbee/ERaFromTempMeas.hpp"
#include "fromZigbee/ERaFromPressMeas.hpp"
#include "fromZigbee/ERaFromHumiMeas.hpp"
#include "fromZigbee/ERaFromZstack.hpp"

#endif /* INC_ERA_FROM_ZIGBEE_HPP_ */
