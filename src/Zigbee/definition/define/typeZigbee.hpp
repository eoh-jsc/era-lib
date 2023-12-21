#ifndef INC_DEFINE_TYPE_ZIGBEE_HPP_
#define INC_DEFINE_TYPE_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum TypeT {
        POLL = 0x00, //0x00
        SREQ = 0x01, //0x20
        AREQ = 0x02, //0x40
        SRSP = 0x03, //0x60
        ERR = 0xFF
    };

    enum SubsystemT {
        RESERVED_INTER = 0x00,
        SYS_INTER = 0x01,
        MAC_INTER = 0x02,
        NWK_INTER = 0x03,
        AF_INTER = 0x04,
        ZDO_INTER = 0x05,
        SAPI_INTER = 0x06,
        UTIL_INTER = 0x07,
        DEBUG_INTER = 0x08,
        APP_INTER = 0x09,
        APP_CNF_INTER = 0x0F,
        GREENPOWER_INTER = 0x15
    };

    enum TypeDeviceT {
        COORDINATOR = 0,
        ROUTERDEVICE = 1,
        ENDDEVICE = 2,
        COMPLEX_DESC_AVAIL = 4,
        USER_DESC_AVAIL = 8,
        RESERVED1 = 16,
        RESERVED2 = 32,
        RESERVED3 = 64,
        RESERVED4 = 128
    };

    enum TypeAnnceDeviceT {
        ANNCE_ENDDEVICE = 0,
        ANNCE_ROUTERDEVICE = 1
    };

};

#endif /* INC_DEFINE_TYPE_ZIGBEE_HPP_ */
