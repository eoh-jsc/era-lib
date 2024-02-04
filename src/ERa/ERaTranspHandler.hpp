#ifndef INC_ERA_TRANSP_HANDLER_HPP_
#define INC_ERA_TRANSP_HANDLER_HPP_

#include <ERa/ERaTransp.hpp>

class ERaTranspHandler
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
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
        ERaTransp* transp = this->firstTransp;
        while (transp != NULL) {
            ERaTransp* next = transp->getNext();
            transp->begin(args);
            transp = next;
        }
    }

    void run() {
        ERaTransp* transp = this->firstTransp;
        while (transp != NULL) {
            ERaTransp* next = transp->getNext();
            transp->run();
            transp = next;
        }
    }

    void publishData(const char* topic,
                    const char* payload) {
        ERaTransp* transp = this->firstTransp;
        while (transp != NULL) {
            ERaTransp* next = transp->getNext();
            transp->publish(topic, payload);
            transp = next;
        }
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

    void setTopic(const char* topic) {
        ERaTransp* transp = this->firstTransp;
        while (transp != NULL) {
            ERaTransp* next = transp->getNext();
            transp->setTopic(topic);
            transp = next;
        }
    }

    void onMessage(MessageCallback_t cb) {
        ERaTransp* transp = this->firstTransp;
        while (transp != NULL) {
            ERaTransp* next = transp->getNext();
            transp->onMessage(cb);
            transp = next;
        }
    }

private:
    ERaTransp* firstTransp;
    ERaTransp* lastTransp;
};

#endif /* INC_ERA_TRANSP_HANDLER_HPP_ */
