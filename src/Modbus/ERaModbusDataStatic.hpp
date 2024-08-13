#ifndef INC_ERA_MODBUS_DATA_STATIC_HPP_
#define INC_ERA_MODBUS_DATA_STATIC_HPP_

#if defined(ERA_MODBUS)

    #include <Modbus/ERaModbusData.hpp>

    #if !defined(ERA_HAS_FUNCTIONAL_H)
        inline
        void ERaModbusData::_onCallback(void* args) {
            ERa.ERaModbusData::onCallback(args);
        }
    #endif

#endif

#endif /* INC_ERA_MODBUS_DATA_STATIC_HPP_ */
