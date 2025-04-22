#ifndef INC_ERA_COMPONENT_HPP_
#define INC_ERA_COMPONENT_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ERa/ERaDetect.hpp>
#include <ERa/ERaData.hpp>

class ERaComponent
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaComponent()
        : mAuthToken(NULL)
        , mBaseTopic(NULL)
        , mCallback(NULL)
        , mNext(NULL)
    {}
    virtual ~ERaComponent()
    {}

    virtual void begin() {}
    virtual void run() {}

    virtual void message(const ERaDataBuff& topic, const char* payload) {
        ERA_FORCE_UNUSED(topic);
        ERA_FORCE_UNUSED(payload);
    }

    void setAuth(const char* auth) {
        this->mAuthToken = auth;
    }

    void setTopic(const char* topic) {
        this->mBaseTopic = topic;
    }

    void onMessage(MessageCallback_t cb) {
        this->mCallback = cb;
    }

    ERaComponent* getNext() const {
        return this->mNext;
    }

    void setNext(ERaComponent& component) {
        this->mNext = &component;
    }

    void setNext(ERaComponent* component) {
        this->mNext = component;
    }

protected:
    const char* mAuthToken;
    const char* mBaseTopic;
    MessageCallback_t mCallback;

private:
    ERaComponent* mNext;
};

#endif /* INC_ERA_COMPONENT_HPP_ */
