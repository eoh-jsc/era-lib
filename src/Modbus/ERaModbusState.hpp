#ifndef INC_ERA_MODBUS_STATE_HPP_
#define INC_ERA_MODBUS_STATE_HPP_

enum ModbusStateT {
    STATE_MB_RUNNING = 0x00,
    STATE_MB_CONTROLLED = 0x01,
    STATE_MB_PARSE = 0x02,
    STATE_MB_SCAN = 0x03,
    STATE_MB_INIT = 0xFF
};

namespace ModbusState {
    class ModbusState {
    public:
        static volatile ModbusStateT& state() {
            static volatile ModbusStateT _state = ModbusStateT::STATE_MB_INIT;
            return _state;
        }
    };

    inline
    ModbusStateT get() {
        return ModbusState::state();
    }

    inline
    bool is(ModbusStateT _state) {
        return ModbusState::state() == _state;
    }

    inline
    void set(ModbusStateT _state) {
        ModbusState::state() = _state;
    }
};

#endif /* INC_ERA_MODBUS_STATE_HPP_ */
