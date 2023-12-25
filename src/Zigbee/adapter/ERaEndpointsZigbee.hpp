#ifndef INC_ERA_ENDPOINTS_ZIGBEE_HPP_
#define INC_ERA_ENDPOINTS_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

/*TODO Init Endpoint*/
static const InitEndpoint_t EpList[] = {
    {EndpointListT::ENDPOINT1, ProfileIDT::ZCL_PROFILE_HOME_AUTOMATION, DeviceIDT::ZCL_DEVICE_CONFIG_TOOL, 0, NetworkLatencyReqT::NO_LATENCY_REQS}
};

#endif /* INC_ERA_ENDPOINTS_ZIGBEE_HPP_ */
