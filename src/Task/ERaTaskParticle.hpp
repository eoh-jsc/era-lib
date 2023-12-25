#ifndef INC_ERA_TASK_PARTICLE_HPP_
#define INC_ERA_TASK_PARTICLE_HPP_

#include <ERa/ERaApi.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        ERA_FORCE_UNUSED(args);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
        ERA_FORCE_UNUSED(args);
    }
#endif

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::apiTask(void* args) {
    ERA_FORCE_UNUSED(args);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initApiTask() {
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initERaApiTask() {
#if defined(ERA_MODBUS)
    Modbus::begin();
#endif
#if defined(ERA_ZIGBEE)
    Zigbee::begin();
#endif
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::runERaApiTask() {
#if defined(ERA_MODBUS)
    Modbus::run();
#endif
#if defined(ERA_ZIGBEE)
    Zigbee::run();
#endif
}

#endif /* INC_ERA_TASK_PARTICLE_HPP_ */
