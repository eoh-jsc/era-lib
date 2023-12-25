#ifndef INC_DEFINE_ENDPOINT_ZIGBEE_HPP_
#define INC_DEFINE_ENDPOINT_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum EndpointListT {
        ENDPOINT_NONE = 0x00,
        ENDPOINT1 = 0x01,
        ENDPOINT2,
        ENDPOINT3,
        ENDPOINT242 = 0xF2,
        ENDPOINT_BROADCAST = 0xFF
    };

};

#endif /* INC_DEFINE_ENDPOINT_ZIGBEE_HPP_ */
