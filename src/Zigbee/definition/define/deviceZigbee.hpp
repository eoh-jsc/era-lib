#ifndef INC_DEFINE_DEVICE_ZIGBEE_HPP_
#define INC_DEFINE_DEVICE_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum PowerSourceT {
        PWS_UNKNOWN0 = 0x00, /* Unknown */
        PWS_SINGLE_PHASE, /* Mains (single phase) */
        PWS_THREE_PHASE, /* Mains (3 phase) */
        PWS_BATTERY, /* Battery */
        PWS_DC_SOURCE, /* DC Source */
        PWS_EMERGENCY_MAIN, /* Emergency mains constantly powered */
        PWS_EMERGENCY_MAIN_SW, /* Emergency mains and transfer switch */
        PWS_NEED_GET = 0xFF
    };

    enum PowerSourceAnnceT {
        PWS_ANNCE_MAIN_POWER = 0x01
    };

    enum DeviceEventT {
        DEVICE_EVENT_JOINED = 0,
        DEVICE_EVENT_ANNOUNCE = 1,
        DEVICE_EVENT_INTERVIEW_STARTED = 2,
        DEVICE_EVENT_INTERVIEW_BASIC_INFO = 3,
        DEVICE_EVENT_INTERVIEW_SUCCESSFUL = 4,
        DEVICE_EVENT_INTERVIEW_FAILED = 5,
        DEVICE_EVENT_LEAVE = 6
    };

};

#endif /* INC_DEFINE_DEVICE_ZIGBEE_HPP_ */
