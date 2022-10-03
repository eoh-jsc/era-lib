#ifndef INC_MVP_STATE_HPP_
#define INC_MVP_STATE_HPP_

enum StateT {
    STATE_WAIT_CONFIG = 0x00,
    STATE_CONFIGURING = 0x01,
    STATE_SWITCH_TO_ETHERNET = 0x02,
    STATE_SWITCH_TO_WIFI = 0x03,
    STATE_SWITCH_TO_AP = 0x04,
    STATE_SWITCH_TO_STA = 0x05,
    STATE_SWITCH_TO_AP_STA = 0x06,
    STATE_CONNECTING_NETWORK = 0x07,
    STATE_CONNECTING_CLOUD = 0x08,
    STATE_CONNECTED = 0x09,
    STATE_RUNNING = 0x0A,
    STATE_OTA_UPGRADE = 0x0B,
    STATE_RESET_CONFIG = 0x0C,
    STATE_RESET_CONFIG_REBOOT = 0x0D,
    STATE_REBOOT = 0x0E,
    STATE_ERROR = 0x0F,
    STATE_MAX = 0xFF
};

namespace MVPState {
    volatile StateT state = StateT::STATE_MAX;

    StateT get() {
        return state;
    }
    bool is(StateT _state) {
        return state == _state;
    }
    void set(StateT _state) {
        state = _state;
    }
};

#endif /* INC_MVP_STATE_HPP_ */