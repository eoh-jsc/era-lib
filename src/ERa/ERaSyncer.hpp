#ifndef INC_ERA_SYNCER_HPP_
#define INC_ERA_SYNCER_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ERa/ERaDetect.hpp>
#include <ERa/ERaData.hpp>

class ERaSyncer
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaSyncer()
        : mAuthToken(NULL)
        , mBaseTopic(NULL)
        , mCallback(NULL)
        , mNext(NULL)
    {}
    virtual ~ERaSyncer()
    {}

    virtual void message(const ERaDataBuff& topic, const char* payload) = 0;

    virtual bool save(const void* data, size_t size) {
        ERA_FORCE_UNUSED(data);
        ERA_FORCE_UNUSED(size);
        return false;
    }

    virtual bool load(void* data, size_t size) {
        ERA_FORCE_UNUSED(data);
        ERA_FORCE_UNUSED(size);
        return false;
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

    ERaSyncer* getNext() const {
        return this->mNext;
    }

    void setNext(ERaSyncer& syncer) {
        this->mNext = &syncer;
    }

    void setNext(ERaSyncer* syncer) {
        this->mNext = syncer;
    }

protected:
    const char* mAuthToken;
    const char* mBaseTopic;
    MessageCallback_t mCallback;

private:
    ERaSyncer* mNext;
};

#endif /* INC_ERA_SYNCER_HPP_ */
