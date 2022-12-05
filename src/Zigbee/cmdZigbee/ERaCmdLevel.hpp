#ifndef INC_ERA_COMMAND_LEVEL_ZIGBEE_HPP_
#define INC_ERA_COMMAND_LEVEL_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::moveToLevelGenLevelCtrl(AFAddrType_t& dstAddr, uint8_t level, uint16_t transtime) {
    vector<uint8_t> payload;
    payload.push_back(level);
    payload.push_back(LO_UINT16(transtime));
    payload.push_back(HI_UINT16(transtime));
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVELEVEL, &payload});
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::stopGenLevelCtrl(AFAddrType_t& dstAddr) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STOP, nullptr});
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::moveToLevelWithOnOffGenLevelCtrl(AFAddrType_t& dstAddr, uint8_t level, uint16_t transtime) {
    vector<uint8_t> payload;
    payload.push_back(level);
    payload.push_back(LO_UINT16(transtime));
    payload.push_back(HI_UINT16(transtime));
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVELEVEL_ONOFF, &payload});
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::stopWithOnOffGenLevelCtrl(AFAddrType_t& dstAddr) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STOP_ONOFF, nullptr});
}

#endif /* INC_ERA_COMMAND_LEVEL_ZIGBEE_HPP_ */
