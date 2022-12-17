#ifndef INC_ERA_ZIGBEE_STATE_HPP_
#define INC_ERA_ZIGBEE_STATE_HPP_

enum ZigbeeStateT {
    STATE_ZB_INIT_SUCCESSFUL = 0x00,
    STATE_ZB_INIT_FAIL = 0x01,
    STATE_ZB_INIT_FORMAT = 0x02,
    STATE_ZB_RUNNING = 0x03,
    STATE_ZB_PERMIT_JOIN = 0x04,
    STATE_ZB_DEVICE_JOINED = 0x05,
    STATE_ZB_DEVICE_INTERVIEWING = 0x06,
    STATE_ZB_FACTORY_RESET = 0x07,
    STATE_ZB_IGNORE = 0xFE,
    STATE_ZB_INIT_MAX = 0xFF
};

namespace ZigbeeState {
    volatile ZigbeeStateT state = ZigbeeStateT::STATE_ZB_INIT_MAX;

    ZigbeeStateT get() {
        return state;
    }
    bool is(ZigbeeStateT _state) {
        return state == _state;
    }
    void set(ZigbeeStateT _state) {
        state = _state;
    }
};

using namespace std;

#endif /* INC_ERA_ZIGBEE_STATE_HPP_ */
