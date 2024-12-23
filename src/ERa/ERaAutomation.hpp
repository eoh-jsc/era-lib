#ifndef INC_ERA_AUTOMATION_HPP_
#define INC_ERA_AUTOMATION_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ERa/ERaDetect.hpp>

class ERaAutomation
{
protected:
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaAutomation()
        : mAuthToken(NULL)
        , mBaseTopic(NULL)
        , mCallback(NULL)
    {
        memset(this->mHashID, 0, sizeof(this->mHashID));
    }
    virtual ~ERaAutomation()
    {}

    virtual void begin(void* args = NULL) = 0;
    virtual void run() = 0;
    virtual void deleteAll() = 0;
    virtual void updateValue(ERaUInt_t configID, double value, bool trigger = false) = 0;

    void setAuth(const char* auth) {
        this->mAuthToken = auth;
    }

    void setTopic(const char* topic) {
        this->mBaseTopic = topic;
    }

    void onMessage(MessageCallback_t cb) {
        this->mCallback = cb;
    }

    bool updateHashID(const char* hash) {
        if (hash == nullptr) {
            return false;
        }
        if (strcmp(this->mHashID, hash)) {
            snprintf(this->mHashID, sizeof(this->mHashID), hash);
            return true;
        }
        return false;
    }

protected:
    const char* mAuthToken;
    const char* mBaseTopic;
    MessageCallback_t mCallback;

    char mHashID[37];
};

#endif /* INC_ERA_AUTOMATION_HPP_ */
