#ifndef INC_ERA_CALLBACKS_HPP_
#define INC_ERA_CALLBACKS_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaData.hpp>
#include <ERa/ERaDebug.hpp>

class ERaServerCallbacks
{
    const char* TAG = "Callbacks";

public:
    ERaServerCallbacks()
    {}
    virtual ~ERaServerCallbacks()
    {}

    virtual void onWrite(const char* topic, const char* payload) {
        ERA_LOG(TAG, ERA_PSTR("onWrite callback default."));
        ERA_FORCE_UNUSED(topic);
        ERA_FORCE_UNUSED(payload);
    }

    virtual void onWrite(const ERaDataBuff& topic, const char* payload) {
        ERA_LOG(TAG, ERA_PSTR("onWrite callback default."));
        ERA_FORCE_UNUSED(topic);
        ERA_FORCE_UNUSED(payload);
    }
};

#endif /* INC_ERA_CALLBACKS_HPP_ */
