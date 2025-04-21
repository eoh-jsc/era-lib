#ifndef INC_ERA_CALLBACKS_HELPER_HPP_
#define INC_ERA_CALLBACKS_HELPER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaAutomationDet.hpp>
#include <ERa/ERaComponent.hpp>
#include <ERa/ERaSyncer.hpp>
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

    virtual void setServerCallbacks(ERaServerCallbacks& rCallbacks) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(rCallbacks);
    }

    virtual void setServerCallbacks(ERaServerCallbacks* pCallbacks) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setServerCallbacks callback default."));
        ERA_FORCE_UNUSED(pCallbacks);
    }

#if defined(ERA_AUTOMATION)
    virtual void setERaAutomation(ERaAutomation& rAutomation) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaAutomation callback default."));
        ERA_FORCE_UNUSED(rAutomation);
    }

    virtual void setERaAutomation(ERaAutomation* pAutomation) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaAutomation callback default."));
        ERA_FORCE_UNUSED(pAutomation);
    }
#endif

    virtual void addERaComponent(ERaComponent& rComponent) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("addERaComponent callback default."));
        ERA_FORCE_UNUSED(rComponent);
    }

    virtual void addERaComponent(ERaComponent* pComponent) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("addERaComponent callback default."));
        ERA_FORCE_UNUSED(pComponent);
    }

    virtual void addERaSyncer(ERaSyncer& rSyncer) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("addERaSyncer callback default."));
        ERA_FORCE_UNUSED(rSyncer);
    }

    virtual void addERaSyncer(ERaSyncer* pSyncer) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("addERaSyncer callback default."));
        ERA_FORCE_UNUSED(pSyncer);
    }

    virtual void setERaTransp(ERaTransp& rTransp) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(rTransp);
    }

    virtual void setERaTransp(ERaTransp* pTransp) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTransp callback default."));
        ERA_FORCE_UNUSED(pTransp);
    }

    virtual void setERaLogger(ERaLogger& rLog) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(rLog);
    }

    virtual void setERaLogger(ERaLogger* pLog) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaLogger callback default."));
        ERA_FORCE_UNUSED(pLog);
    }

    ERaApiHandler& Api;
};

#endif /* INC_ERA_CALLBACKS_HELPER_HPP_ */
