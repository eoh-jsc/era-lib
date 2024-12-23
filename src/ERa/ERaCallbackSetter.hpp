#ifndef INC_ERA_CALLBACKS_HELPER_HPP_
#define INC_ERA_CALLBACKS_HELPER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaAutomationDet.hpp>
#include <ERa/ERaComponent.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaLogger.hpp>
#include <ERa/ERaCallbacks.hpp>

class ERaApiHandler;

class ERaCallbackSetter
{
    const char* TAG = "Callbacks";

public:
    ERaCallbackSetter(ERaApiHandler& api)
        : Api(api)
    {}
    virtual ~ERaCallbackSetter()
    {}

    virtual void setServerCallbacks(ERaServerCallbacks& callbacks) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(callbacks);
    }

    virtual void setServerCallbacks(ERaServerCallbacks* pCallbacks) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(pCallbacks);
    }

#if defined(ERA_AUTOMATION)
    virtual void setERaAutomation(ERaAutomation& _automation) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaAutomation callback default."));
        ERA_FORCE_UNUSED(_automation);
    }

    virtual void setERaAutomation(ERaAutomation* _pAutomation) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaAutomation callback default."));
        ERA_FORCE_UNUSED(_pAutomation);
    }
#endif

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

    ERaApiHandler& Api;
};

#endif /* INC_ERA_CALLBACKS_HELPER_HPP_ */
