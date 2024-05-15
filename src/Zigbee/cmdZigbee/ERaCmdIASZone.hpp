#ifndef INC_ERA_COMMAND_IAS_ZONE_ZIGBEE_HPP_
#define INC_ERA_COMMAND_IAS_ZONE_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::enrollResponseIASZone(AFAddrType_t& dstAddr, uint8_t enrollRspCode, uint8_t zoneId) {
    std::vector<uint8_t> payload;
    payload.push_back(enrollRspCode);
    payload.push_back(zoneId);

    return this->thisToZigbee().sendCommandIdZigbee(this->zclHeader11, dstAddr, EndpointListT::ENDPOINT1,
                                                    {ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE,
                                                    ZbZclIasZoneClientCommandsT::ZCL_IAS_ZONE_CLI_CMD_ZONE_ENROLL_RESPONSE, &payload});
}

#endif /* INC_ERA_COMMAND_IAS_ZONE_ZIGBEE_HPP_ */
