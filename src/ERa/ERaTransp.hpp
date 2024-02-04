#ifndef INC_ERA_TRANSP_HPP_
#define INC_ERA_TRANSP_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>

class ERaTransp
    : public Client
{
protected:
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaTransp()
        : topic(NULL)
        , callback(NULL)
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

    void setTopic(const char* _topic) {
        this->topic = _topic;
    }

    void onMessage(MessageCallback_t cb) {
        this->callback = cb;
    }

protected:
    const char* topic;
    MessageCallback_t callback;

private:
    ERaTransp* next;
};

#endif /* INC_ERA_TRANSP_HPP_ */
