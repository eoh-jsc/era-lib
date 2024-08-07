#ifndef INC_DEFINE_ZB_ZIGBEE_HPP_
#define INC_DEFINE_ZB_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum ZBCommandsT {
        ZB_START_REQUEST = 0x00,
        ZB_BIND_DEVICE,
        ZB_ALLOW_BIND,
        ZB_SEND_DATA_REQUEST,
        ZB_READ_CONFIGURATION,
        ZB_WRITE_CONFIGURATION,
        ZB_GET_DEVICE_INFO,
        ZB_FIND_DEVICE_REQUEST,
        ZB_PERMIT_JOINING_REQUEST,
        ZB_SYSTEM_RESET,
        ZB_START_CONFIRM = 0x80,
        ZB_BIND_CONFIRM,
        ZB_ALLOW_BIND_CONFIRM,
        ZB_SEND_DATA_CONFIRM,
        ZB_FIND_DEVICE_CONFIRM = 0x85,
        ZB_RECEIVE_DATA_INDICATION = 0x87
    };

} /* namespace ZigbeeNamespace */

#endif /* INC_DEFINE_ZB_ZIGBEE_HPP_ */
