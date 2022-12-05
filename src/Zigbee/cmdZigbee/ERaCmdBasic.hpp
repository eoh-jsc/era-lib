#ifndef INC_ERA_COMMAND_BASIC_ZIGBEE_HPP_
#define INC_ERA_COMMAND_BASIC_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::resetFactoryDefaultsGenBasic(AFAddrType_t& dstAddr) {
    if (dstAddr.addrMode == AddressModeT::ADDR_GROUP) {
        return ResultT::RESULT_FAIL;
    }
    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_BASIC, ZbZclBasicSvrCmdT::ZCL_BASIC_RESET_FACTORY, nullptr});
}

#endif /* INC_ERA_COMMAND_BASIC_ZIGBEE_HPP_ */
