#ifndef INC_DEFINE_AF_ZIGBEE_HPP_
#define INC_DEFINE_AF_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum InterpanCtlT {
        CTL_INTERPAN = 0,
        SET_INTERPAN,
        REG_INTERPAN,
        CHK_INTERPAN
    };

    enum NetworkLatencyReqT {
        NO_LATENCY_REQS = 0,
        FAST_BEACONS,
        SLOW_BEACONS
    };

    enum OptionsAFT {
        DEFAULT_OP = 0,
        PREPROCESS = 4,
        LIMIT_CONCENTRATOR = 8,
        ACK_REQUEST = 16,
        DISCV_ROUTE = 32,
        EN_SECURITY = 64,
        SKIP_ROUTING = 128
    };

    enum AFCommandsT {
        AF_REGISTER = 0x00,
        AF_DATA_REQUEST,
        AF_DATA_REQUEST_EXT,
        AF_DATA_REQUEST_SRC_RTG,
        AF_INTER_PAN_CTL = 0x10,
        AF_DATA_STORE,
        AF_DATA_RETRIEVE,
        AF_APSF_CONFIG_SET,
        AF_DATA_CONFIRM = 0x80,
        AF_INCOMING_MSG,
        AF_INCOMING_MSG_EXT,
        AF_REFLECT_ERROR
    };

};

#endif /* INC_DEFINE_AF_ZIGBEE_HPP_ */
