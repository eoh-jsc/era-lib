#ifndef INC_ERA_MODBUS_CALLBACKS_HPP_
#define INC_ERA_MODBUS_CALLBACKS_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <Modbus/ERaModbusTransp.hpp>

class ERaModbusCallbacks
{
    const char* TAG = "Callbacks";

public:
    ERaModbusCallbacks()
    {}
    virtual ~ERaModbusCallbacks()
    {}

    virtual void onData(const ERaModbusRequest* request,
                        const ERaModbusResponse* response,
                        const ModbusBuffer_t* pData) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("onData callback default."));
        ERA_FORCE_UNUSED(request);
        ERA_FORCE_UNUSED(response);
        ERA_FORCE_UNUSED(pData);
    }

    virtual void onError(const ERaModbusRequest* request) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("onError callback default."));
        ERA_FORCE_UNUSED(request);
    }

    virtual void onAction(const ModbusAction_t* request,
                          const Action_t* action,
                          ModbusConfig_t* config) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("onAction callback default."));
        ERA_FORCE_UNUSED(request);
        ERA_FORCE_UNUSED(action);
        ERA_FORCE_UNUSED(config);
    }
};

#endif /* INC_ERA_MODBUS_CALLBACKS_HPP_ */
