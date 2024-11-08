#ifndef INC_ERA_CALLBACKS_HELPER_HPP_
#define INC_ERA_CALLBACKS_HELPER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaComponent.hpp>
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
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(callbacks);
    }

    virtual void setServerCallbacks(ERaServerCallbacks* pCallbacks) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(pCallbacks);
    }

    virtual void setERaComponent(ERaComponent& _component) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaComponent callback default."));
        ERA_FORCE_UNUSED(_component);
    }

    virtual void setERaComponent(ERaComponent* _pComponent) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaComponent callback default."));
        ERA_FORCE_UNUSED(_pComponent);
    }

    virtual void setERaTransp(ERaTransp& _transp) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(_transp);
    }

    virtual void setERaTransp(ERaTransp* _pTransp) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(_pTransp);
    }

    virtual void setERaLogger(ERaLogger& _logger) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(_logger);
    }

    virtual void setERaLogger(ERaLogger* _pLogger) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(_pLogger);
    }
};

#endif /* INC_ERA_CALLBACKS_HELPER_HPP_ */
