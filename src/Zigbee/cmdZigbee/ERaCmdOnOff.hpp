#ifndef INC_ERA_COMMAND_ONOFF_ZIGBEE_HPP_
#define INC_ERA_COMMAND_ONOFF_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::onoffGenOnOff(AFAddrType_t& dstAddr, uint8_t state) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_ONOFF, state, nullptr});
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::onWithTimeOffGenOnOff(AFAddrType_t& dstAddr, uint8_t ctrlBits, uint16_t onTime, uint16_t offWaitTime) {
    vector<uint8_t> payload;
    payload.push_back(ctrlBits);
    payload.push_back(LO_UINT16(onTime));
    payload.push_back(HI_UINT16(onTime));
    payload.push_back(LO_UINT16(offWaitTime));
    payload.push_back(HI_UINT16(offWaitTime));
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_ONOFF, ZbZclOnOffSvrCmdT::ZCL_ONOFF_ON_WITH_TIMED_OFF, &payload});
}

#endif /* INC_ERA_COMMAND_ONOFF_ZIGBEE_HPP_ */
