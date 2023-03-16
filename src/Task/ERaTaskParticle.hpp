#ifndef INC_ERA_TASK_PARTICLE_HPP_
#define INC_ERA_TASK_PARTICLE_HPP_

#include <ERa/ERaProtocol.hpp>

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

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::runRead();
	Base::Modbus::runWrite();
#endif
}

#endif /* INC_ERA_TASK_PARTICLE_HPP_ */
