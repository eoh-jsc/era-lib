#ifndef INC_ERA_COMMAND_ZIGBEE_HPP_
#define INC_ERA_COMMAND_ZIGBEE_HPP_

#include <stdint.h>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>
#include "definition/ERaDefineZigbee.hpp"
#include "utility/ERaUtilityZigbee.hpp"

template <class ToZigbee>
class ERaCommandZigbee
{
protected:
    const uint8_t SkipBootByte = 0xEF;
    const uint16_t AddrBroadcastJoin = NWK_ADDR_BROADCAST_JOIN;
    const uint16_t AddrBroadcastAll = NWK_ADDR_BROADCAST_GP;

public:
    ERaCommandZigbee()
        : zclHeader01 {
            .frameType = FrameTypeT::SPECIFIC,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::CLIENT_TO_SERVER,
            .disableRsp = false,
            .reservedBits = 0
        }
        , zclHeader10 {
            .frameType = FrameTypeT::GLOBAL,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::CLIENT_TO_SERVER,
            .disableRsp = true,
            .reservedBits = 0
        }
        , zclHeader11 {
            .frameType = FrameTypeT::SPECIFIC,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::CLIENT_TO_SERVER,
            .disableRsp = true,
            .reservedBits = 0
        }
        , zclHeader15 {
            .frameType = FrameTypeT::SPECIFIC,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::CLIENT_TO_SERVER,
            .disableRsp = true,
            .reservedBits = 0
        }
        , zclHeader18 {
            .frameType = FrameTypeT::GLOBAL,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::SERVER_TO_CLIENT,
            .disableRsp = true,
            .reservedBits = 0
        }
        , zclHeader19 {
            .frameType = FrameTypeT::SPECIFIC,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::SERVER_TO_CLIENT,
            .disableRsp = true,
            .reservedBits = 0
        }
        , zclHeaderRsp {
            .frameType = FrameTypeT::GLOBAL,
            .manufCode = ManufacturerCodesT::MANUF_CODE_NONE,
            .direction = DirectionT::SERVER_TO_CLIENT,
            .disableRsp = true,
            .reservedBits = 0
        }
        , device(InfoDevice_t::instance())
        , coordinator(InfoCoordinator_t::instance())
    {}
    ~ERaCommandZigbee()
    {}

protected:
    void skipBootloader();
    ResultT defaultResponse(const DefaultRsp_t& defaultRsp, uint8_t statusCode);
    ResultT readAttributes(AFAddrType_t& dstAddr,
                            EndpointListT srcEndpoint,
                            ClusterIDT zclId,
                            ManufacturerCodesT manufCode,
                            vector<uint16_t> attributes,
                            uint8_t cmdWait = AFCommandsT::AF_DATA_CONFIRM,
                            void* value = nullptr);
    ResultT readAttributesResponse(AFAddrType_t& dstAddr,
                            EndpointListT srcEndpoint,
                            ClusterIDT zclId,
                            ManufacturerCodesT manufCode,
                            vector<DataReadAttrRsp_t> data,
                            uint8_t cmdWait = AFCommandsT::AF_DATA_CONFIRM,
                            void* _transId = nullptr);
    ResultT writeAttributes(AFAddrType_t& dstAddr,
                            EndpointListT srcEndpoint,
                            ClusterIDT zclId,
                            ManufacturerCodesT manufCode,
                            vector<DataWriteAttr_t> data,
                            uint8_t cmdWait = AFCommandsT::AF_DATA_CONFIRM);
    ResultT writeAttributesResponse(AFAddrType_t& dstAddr,
                            EndpointListT srcEndpoint,
                            ClusterIDT zclId,
                            ManufacturerCodesT manufCode,
                            vector<uint16_t> attributes,
                            uint8_t cmdWait,
                            void* _transId = nullptr);
    ResultT requestBind(AFAddrType_t& srcAddr,
                        AFAddrType_t& dstAddr,
                        ClusterIDT zclId);
    ResultT requestUnbind(AFAddrType_t& srcAddr,
                        AFAddrType_t& dstAddr,
                        ClusterIDT zclId);

    ResultT requestSimpleDesc(AFAddrType_t& dstAddr, size_t limit, void* value = nullptr);
    ResultT requestNodeDesc(AFAddrType_t& dstAddr, size_t limit, void* value = nullptr);
    ResultT extRouterDiscovery(AFAddrType_t& dstAddr, const OptionsAFT options, const uint8_t radius);
    ResultT requestListEndpoint(AFAddrType_t& dstAddr, uint16_t nwkAddrOfInterest, size_t limit, void* value = nullptr);
    ResultT removeDevice(AFAddrType_t& dstAddr, bool rejoin = false, bool removeChildren = false, bool force = false);
    void removeAllDevice(bool rejoin = false, bool removeChildren = false, bool force = false);

    ResultT pingSystem(size_t limit, uint32_t timeout = MAX_TIMEOUT, void (*feed)(void) = nullptr);
    ResultT versionSystem(size_t limit, void* value = nullptr);
    ResultT getExtAddrSystem(size_t limit, void* value = nullptr);

    ResultT nwkKeyReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT znpHasConfiguredReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT extAddrReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT nibReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT waitNibReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT channelListReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT panIdReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT extendedPanIdReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT activeKeyReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT alternKeyReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT apsExtPanIdReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT apsLinkKeyReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT enablePrecfgkeysReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT precfgkeysReadConfigZstack(size_t limit, void* value = nullptr);
    ResultT precfgkeysReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT precfgkeysReadZstack(size_t limit, void* value = nullptr);
    ResultT touchlinkKeyReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT materialTableReadOSALZstack(size_t limit, void* value = nullptr);
    ResultT getDeviceInfoZstack(size_t limit, void* value = nullptr);
    ResultT stateStartupAppZstack(size_t limit, void* value = nullptr);
    ResultT requestListEndpointZstack(uint16_t nwkAddr, uint16_t nwkAddrOfinterest, size_t limit, void* value = nullptr);
    ResultT findGroupExtZDO(EndpointListT endpoint, uint16_t groupId, size_t limit, void* value = nullptr);
    ResultT addGroupExtZDO(EndpointListT endpoint, uint16_t groupId, char* groupName = nullptr);
    ResultT addGroupExtZDOZstack(EndpointListT endpoint, uint16_t groupId, void* value = nullptr, bool format = false);

    template <int size>
    ResultT requestSimpleDescZstack(const InitEndpoint_t(&epList)[size], void* value = nullptr);
    ResultT afRegisterEndpointZstack(const InitEndpoint_t& epInfo);
    template <int size>
    ResultT afRegisterEndpointListZstack(const InitEndpoint_t(&epList)[size]);

    ResultT requestMatchDescZstack() {
        //upload soon
        return ResultT::RESULT_SUCCESSFUL;
    }
    ResultT requestNwkAddrZstack(AFAddrType_t& dstAddr, uint8_t reqType, uint8_t startIndex, void* value = nullptr);
    ResultT requestIEEEAddrZstack(AFAddrType_t& dstAddr, uint8_t reqType, uint8_t startIndex, void* value = nullptr);
    ResultT requestInfoNwkExtZstack(size_t limit, void* value = nullptr);
    ResultT requestResetZstack(ResetTypeT type, size_t limit, void* value = nullptr);

    ResultT startupOptionWriteOSALZstack(uint8_t option = 0x02);
    ResultT logicalTypeWriteOSALZstack(TypeDeviceT type);
    ResultT enablePrecfgkeysWriteOSALZstack(bool distribute = false);
    ResultT zdoDirectWriteOSALZstack(bool enable = true);

    template <int size>
    ResultT activeKeyWriteOSALZstack(const uint8_t(&key)[size]); /* For 3.0 */
    template <int size>
    ResultT alternKeyWriteOSALZstack(const uint8_t(&key)[size]); /* For 3.0 */
    template <int size>
    ResultT apsExtPanIdWriteOSALZstack(const uint8_t(&extPanId)[size]);

    ResultT materialTableWriteOSALZstack(const uint8_t* pData, size_t pDataLen);
    ResultT channelListWriteOSALZstack(const ZBChannelT channel);
    ResultT panIdWriteOSALZstack(const uint16_t panId);

    template <int size>
    ResultT extendedPanIdWriteOSALZstack(const uint8_t(&extPanId)[size]);
    template <int size>
    ResultT precfgkeysWriteConfigZstack(const uint8_t(&key)[size]); /* For 1.2 */
    template <int size>
    ResultT precfgkeysWriteOSALZstack(const uint8_t(&key)[size]); /* For 3.0 */
    template <int size>
    ResultT precfgkeysWriteZstack(const uint8_t(&key)[size]);
    template <int size>
    ResultT touchlinkKeyWriteOSALZstack(const uint8_t(&key)[size]); /* For 1.2 */
    ResultT znpHasConfiguredWriteOSALInitZstack();
    ResultT znpHasConfiguredWriteOSALZstack();

    ResultT touchlinkStartCommZstack(); /* For 3.0 */
    ResultT netSteeringStartCommZstack(); /* For 3.0 */
    ResultT netFormationStartCommZstack(); /* For 3.0 */
    ResultT findAndBindingStartCommZstack(); /* For 3.0 */
    ResultT startCommissioningZstack(uint8_t mode); /* For 3.0 */

    ResultT setBDBChannelZstack(ZBChannelT channel, bool isPrimary); /* For 3.0 */
    ResultT setTuningOperationZstack(uint8_t operation, uint8_t value); /* For 3.0 */

    ResultT readItemZstack(NvItemsIdsT itemId, uint16_t offset, void* value = nullptr);
    ResultT readItemZstacks(NvItemsIdsT itemId, uint16_t offset, size_t limit, void* value = nullptr);
    ResultT writeItemZstack(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data);
    ResultT updateItemZstack(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data, void* value = nullptr);
    ResultT deleteItemZstack(NvItemsIdsT itemId);
    ResultT readOSALLength(NvItemsIdsT itemId, void* value = nullptr);
    ResultT readOSALItem(NvItemsIdsT itemId, uint8_t offset, void* value = nullptr);
    ResultT readOSALItemExt(NvItemsIdsT itemId, uint16_t offset, void* value = nullptr);
    ResultT writeOSALItem(NvItemsIdsT itemId, uint8_t offset, vector<uint8_t>& data, void* value = nullptr);
    ResultT writeOSALItemExt(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data, void* value = nullptr);
    ResultT writeOSALItemInit(NvItemsIdsT itemId, uint16_t itemLen, vector<uint8_t>& data);
    ResultT deleteOSALItem(NvItemsIdsT itemId, uint16_t len);

    ResultT readNVItemZstack(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, void* value = nullptr);
    ResultT writeNVItemZstack(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, vector<uint8_t>& data);
    ResultT readNVLength(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, void* value = nullptr);
    ResultT readNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, uint8_t len, void* value = nullptr);
    ResultT writeNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, vector<uint8_t>& data, void* value = nullptr);
    ResultT deleteNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId);

    ResultT permitJoinRequest(AFAddrType_t& dstAddr, uint8_t seconds);

    ResultT resetFactoryDefaultsGenBasic(AFAddrType_t& dstAddr);

    ResultT onoffGenOnOff(AFAddrType_t& dstAddr, uint8_t state);
    ResultT onWithTimeOffGenOnOff(AFAddrType_t& dstAddr, uint8_t ctrlBits, uint16_t onTime, uint16_t offWaitTime);

    ResultT moveToLevelGenLevelCtrl(AFAddrType_t& dstAddr, uint8_t level, uint16_t transtime);
    ResultT stopGenLevelCtrl(AFAddrType_t& dstAddr);
    ResultT moveToLevelWithOnOffGenLevelCtrl(AFAddrType_t& dstAddr, uint8_t level, uint16_t transtime);
    ResultT stopWithOnOffGenLevelCtrl(AFAddrType_t& dstAddr);

private:
    inline
    const ToZigbee& thisToZigbee() const {
        return static_cast<const ToZigbee&>(*this);
    }

    inline
    ToZigbee& thisToZigbee() {
        return static_cast<ToZigbee&>(*this);
    }

    HeaderZclFrame_t zclHeader01;
    HeaderZclFrame_t zclHeader10;
    HeaderZclFrame_t zclHeader11;
    HeaderZclFrame_t zclHeader15;
    HeaderZclFrame_t zclHeader18;
    HeaderZclFrame_t zclHeader19;
    HeaderZclFrame_t zclHeaderCus;
    HeaderZclFrame_t zclHeaderRsp;

    InfoDevice_t*& device;
    InfoCoordinator_t*& coordinator;
};

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::defaultResponse(const DefaultRsp_t& defaultRsp, uint8_t statusCode) {
    AFAddrType_t dstAddr {
        .addr = {
            .nwkAddr = defaultRsp.dstAddr
        },
        .addrMode = AddressModeT::ADDR_16BIT,
        .endpoint = defaultRsp.dstEndpoint,
        .panId = 0
    };
    DirectionT direction = static_cast<DirectionT>((defaultRsp.frameCtrl >> 3) & 0x01);
    bool disableRsp = (defaultRsp.frameCtrl >> 4) & 0x01;
    if(disableRsp || defaultRsp.hasResponse) {
        return ResultT::RESULT_FAIL;
    }
    this->zclHeaderRsp.manufCode = defaultRsp.manufCode;
    if(direction == DirectionT::CLIENT_TO_SERVER) {
        this->zclHeaderRsp.direction = DirectionT::SERVER_TO_CLIENT;
    }
    else {
        this->zclHeaderRsp.direction = DirectionT::CLIENT_TO_SERVER;
    }
    vector<uint8_t> payload;
    payload.push_back(defaultRsp.cmdId);
    payload.push_back(statusCode);
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeaderRsp, dstAddr,
                                                    defaultRsp.srcEndpoint, {defaultRsp.zclId, CommandIdT::DEFAULT_RSP, &payload},
                                                    AFCommandsT::AF_DATA_CONFIRM, nullptr,
                                                    DEFAULT_TIMEOUT, const_cast<uint8_t*>(&defaultRsp.transId));
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::permitJoinRequest(AFAddrType_t& dstAddr, uint8_t seconds) {
    vector<uint8_t> payload;
    payload.push_back(dstAddr.addrMode);
    payload.push_back(LO_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(HI_UINT16(dstAddr.addr.nwkAddr));
    payload.push_back(seconds);
    payload.push_back(0x00); /* Trust Center Significance */
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::ZDO_INTER, ZDOCommandsT::ZDO_MGMT_PERMIT_JOIN_REQ,
                                                    TypeT::AREQ, ZDOCommandsT::ZDO_MGMT_PERMIT_JOIN_RSP); /* Set the Permit Join for the destination device */
}

#include "cmdZigbee/ERaCmdDevice.hpp"
#include "cmdZigbee/ERaCmdZstack.hpp"
#include "cmdZigbee/ERaCmdNv.hpp"
#include "cmdZigbee/ERaCmdBasic.hpp"
#include "cmdZigbee/ERaCmdLevel.hpp"
#include "cmdZigbee/ERaCmdOnOff.hpp"

#endif /* INC_ERA_COMMAND_ZIGBEE_HPP_ */
