#ifndef INC_ERA_MODBUS_STATE_HPP_
#define INC_ERA_MODBUS_STATE_HPP_

enum ModbusStateT {
    STATE_MB_RUNNING = 0x00,
    STATE_MB_CONTROLLED = 0x01,
    STATE_MB_PARSE = 0x02,
    STATE_MB_INIT = 0xFF
};

namespace ModbusState {
    volatile ModbusStateT state = ModbusStateT::STATE_MB_INIT;

    ModbusStateT get() {
        return state;
    }
    bool is(ModbusStateT _state) {
        return state == _state;
    }
    void set(ModbusStateT _state) {
        state = _state;
    }
};

#endif /* INC_ERA_MODBUS_STATE_HPP_ */
