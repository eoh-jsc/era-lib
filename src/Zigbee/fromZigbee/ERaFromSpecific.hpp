#ifndef INC_ERA_FROM_SPECIFIC_ZIGBEE_HPP_
#define INC_ERA_FROM_SPECIFIC_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
void ERaFromZigbee<Zigbee>::processFrameTypeSpecific(const DataAFMsg_t& afMsg, DefaultRsp_t& defaultRsp, void* value) {
    defaultRsp.attribute = 0xFFFF;
    defaultRsp.cmdId = afMsg.cmdId;

    IdentDeviceAddr_t* deviceInfo = this->createDataSpecific(afMsg, defaultRsp);

    switch (afMsg.zclId) {
        case ClusterIDT::ZCL_CLUSTER_OTA_UPGRADE:
            break;
        case ClusterIDT::ZCL_CLUSTER_POLL_CONTROL:
            break;
        case ClusterIDT::ZCL_CLUSTER_GREEN_POWER:
            break;
        case ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE:
            break;
        default:
            break;
    }

    ERA_FORCE_UNUSED(deviceInfo);
}

#endif /* INC_ERA_FROM_SPECIFIC_ZIGBEE_HPP_ */
