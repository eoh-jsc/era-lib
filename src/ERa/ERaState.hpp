#ifndef INC_ERA_STATE_HPP_
#define INC_ERA_STATE_HPP_

enum StateT {
    STATE_WAIT_CONFIG = 0x00,
    STATE_CONFIGURING = 0x01,
    STATE_SWITCH_TO_ETHERNET = 0x02,
    STATE_SWITCH_TO_WIFI = 0x03,
    STATE_SWITCH_TO_AP = 0x04,
    STATE_SWITCH_TO_STA = 0x05,
    STATE_SWITCH_TO_AP_STA = 0x06,
    STATE_SCAN_DEVICE = 0x07,
    STATE_CONNECTING_NETWORK = 0x08,
    STATE_CONNECTING_NEW_NETWORK = 0x09,
    STATE_CONNECTING_CLOUD = 0x0A,
    STATE_CONNECTED = 0x0B,
    STATE_RUNNING = 0x0C,
    STATE_DISCONNECTED = 0x0D,
    STATE_OTA_UPGRADE = 0x0E,
    STATE_RESET_CONFIG = 0x0F,
    STATE_RESET_CONFIG_REBOOT = 0x10,
    STATE_REQUEST_LIST_WIFI = 0x11,
    STATE_REBOOT = 0x12,
    STATE_ERROR = 0x13,
    STATE_MAX = 0xFF
};

namespace ERaState {
    class ERaState {
    public:
        static volatile StateT& state() {
            static volatile StateT _state = StateT::STATE_MAX;
            return _state;
        }
    };

    inline
    StateT get() {
        return ERaState::state();
    }

    inline
    bool is(StateT _state) {
        return ERaState::state() == _state;
    }

    inline
    bool isConfiguring() {
        return (ERaState::state() == StateT::STATE_WAIT_CONFIG) ||
               (ERaState::state() == StateT::STATE_CONFIGURING) ||
               (ERaState::state() == StateT::STATE_SCAN_DEVICE);
    }

    inline
    void set(StateT _state) {
        ERaState::state() = _state;
    }
} /* namespace ERaState */

#endif /* INC_ERA_STATE_HPP_ */
