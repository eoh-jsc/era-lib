#ifndef INC_ERA_TRANSP_HANDLER_HPP_
#define INC_ERA_TRANSP_HANDLER_HPP_

#include <ERa/ERaTransp.hpp>

#define ERA_TEMPLATABLE_TRANSP_LOOP(method, ...)    \
    ERaTransp* transp = this->firstTransp;          \
    while (transp != NULL) {                        \
        ERaTransp* next = transp->getNext();        \
        transp->method(__VA_ARGS__);                \
        transp = next;                              \
    }

class ERaTranspHandler
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> StateCallback_t;
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*StateCallback_t)(void);
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaTranspHandler()
        : firstTransp(NULL)
        , lastTransp(NULL)
    {}
    ~ERaTranspHandler()
    {}

protected:
    void begin(void* args = NULL) {
        ERA_TEMPLATABLE_TRANSP_LOOP(begin, args)
    }

    void run() {
        ERA_TEMPLATABLE_TRANSP_LOOP(run)
    }

    void publishData(const char* topic,
                    const char* payload) {
        ERA_TEMPLATABLE_TRANSP_LOOP(publish, topic, payload)
    }

    void addTransp(ERaTransp& transp) {
        this->addTransp(&transp);
    }

    void addTransp(ERaTransp* pTransp) {
        if (this->lastTransp == NULL) {
            this->firstTransp = pTransp;
        }
        else {
            this->lastTransp->setNext(pTransp);
        }
        this->lastTransp = pTransp;
    }

    void setAuth(const char* auth) {
        ERA_TEMPLATABLE_TRANSP_LOOP(setAuth, auth)
    }

    void setTopic(const char* topic) {
        ERA_TEMPLATABLE_TRANSP_LOOP(setTopic, topic)
    }

    void onMessage(MessageCallback_t cb) {
        ERA_TEMPLATABLE_TRANSP_LOOP(onMessage, cb)
    }

    void onStateChange(StateCallback_t onCb,
                       StateCallback_t offCb) {
        ERA_TEMPLATABLE_TRANSP_LOOP(onStateChange, onCb, offCb)
    }

private:
    ERaTransp* firstTransp;
    ERaTransp* lastTransp;
};

#endif /* INC_ERA_TRANSP_HANDLER_HPP_ */
