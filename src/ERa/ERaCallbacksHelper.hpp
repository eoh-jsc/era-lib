#ifndef INC_ERA_CALLBACKS_HELPER_HPP_
#define INC_ERA_CALLBACKS_HELPER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaLogger.hpp>
#include <ERa/ERaCallbacks.hpp>

class ERaCallbacksHelper
{
    const char* TAG = "Callbacks";

public:
    ERaCallbacksHelper()
    {}
    virtual ~ERaCallbacksHelper()
    {}

    virtual void setServerCallbacks(ERaServerCallbacks& callbacks) {
        ERA_LOG(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(callbacks);
    }

    virtual void setServerCallbacks(ERaServerCallbacks* pCallbacks) {
        ERA_LOG(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(pCallbacks);
    }

    virtual void setERaTransp(ERaTransp& _transp) {
        ERA_LOG(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(_transp);
    }

    virtual void setERaTransp(ERaTransp* _pTransp) {
        ERA_LOG(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(_pTransp);
    }

    virtual void setERaLogger(ERaLogger& _logger) {
        ERA_LOG(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(_logger);
    }

    virtual void setERaLogger(ERaLogger* _pLogger) {
        ERA_LOG(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(_pLogger);
    }
};

#endif /* INC_ERA_CALLBACKS_HELPER_HPP_ */
