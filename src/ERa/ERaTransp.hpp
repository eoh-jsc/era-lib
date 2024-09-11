#ifndef INC_ERA_TRANSP_HPP_
#define INC_ERA_TRANSP_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Client.hpp>
#include <ERa/ERaDetect.hpp>

class ERaTransp
    : public Client
{
protected:
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> StateCallback_t;
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*StateCallback_t)(void);
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaTransp()
        : authToken(NULL)
        , baseTopic(NULL)
        , callback(NULL)
        , connectedCb(NULL)
        , disconnectedCb(NULL)
        , next(NULL)
    {}
    virtual ~ERaTransp()
    {}

    virtual void begin(void* args = NULL) = 0;
    virtual void run() = 0;
    virtual bool publish(const char* topic,
                        const char* payload) {
        ERA_FORCE_UNUSED(topic);
        ERA_FORCE_UNUSED(payload);
        return true;
    }

    ERaTransp* getNext() const {
        return this->next;
    }

    void setNext(ERaTransp& transp) {
        this->next = &transp;
    }

    void setNext(ERaTransp* transp) {
        this->next = transp;
    }

    void setAuth(const char* auth) {
        this->authToken = auth;
    }

    void setTopic(const char* topic) {
        this->baseTopic = topic;
    }

    void onMessage(MessageCallback_t cb) {
        this->callback = cb;
    }

    void onStateChange(StateCallback_t onCb,
                       StateCallback_t offCb) {
        this->connectedCb = onCb;
        this->disconnectedCb = offCb;
    }

protected:
    const char* authToken;
    const char* baseTopic;
    MessageCallback_t callback;
    StateCallback_t connectedCb;
    StateCallback_t disconnectedCb;

private:
    ERaTransp* next;
};

#endif /* INC_ERA_TRANSP_HPP_ */
