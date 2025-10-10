#ifndef INC_ERA_CALLBACK_SETTER_HPP_
#define INC_ERA_CALLBACK_SETTER_HPP_

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

    virtual ERaLogger* getERaLogger() const {
        ERA_LOG_WARNING(TAG, ERA_PSTR("getERaLogger callback default."));
        return nullptr;
    }

    virtual void setERaTime(ERaTime& rTime, bool enableUpdate = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTime callback default."));
        ERA_FORCE_UNUSED(rTime);
        ERA_FORCE_UNUSED(enableUpdate);
    }

    virtual void setERaTime(ERaTime* pTime, bool enableUpdate = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("setERaTime callback default."));
        ERA_FORCE_UNUSED(pTime);
        ERA_FORCE_UNUSED(enableUpdate);
    }

    virtual ERaTime* getERaTime() const {
        ERA_LOG_WARNING(TAG, ERA_PSTR("getERaTime callback default."));
        return nullptr;
    }

    ERaApiHandler& Api;
};

#endif /* INC_ERA_CALLBACK_SETTER_HPP_ */
