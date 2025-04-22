#ifndef INC_ERA_COMPONENT_HANDLER_HPP_
#define INC_ERA_COMPONENT_HANDLER_HPP_

#include <ERa/ERaComponent.hpp>

#define ERA_TEMPLATABLE_COMPONENT_LOOP(method, ...)  \
    ERaComponent* component = this->mFirstComponent; \
    while (component != NULL) {                      \
        ERaComponent* next = component->getNext();   \
        component->method(__VA_ARGS__);              \
        component = next;                            \
    }

class ERaComponentHandler
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaComponentHandler()
        : mFirstComponent(NULL)
        , mLastComponent(NULL)
        , mInitialized(false)
    {}
    ~ERaComponentHandler()
    {}

protected:
    void begin() {
        if (this->mInitialized) {
            return;
        }
        ERA_TEMPLATABLE_COMPONENT_LOOP(begin);
        this->mInitialized = true;
    }

    void run() {
        ERA_TEMPLATABLE_COMPONENT_LOOP(run);
    }

    void message(const ERaDataBuff& topic, const char* payload) {
        ERA_TEMPLATABLE_COMPONENT_LOOP(message, topic, payload)
    }

    void addComponent(ERaComponent& component) {
        this->addComponent(&component);
    }

    void addComponent(ERaComponent* pComponent) {
        if (this->mLastComponent == NULL) {
            this->mFirstComponent = pComponent;
        }
        else {
            this->mLastComponent->setNext(pComponent);
        }
        this->mLastComponent = pComponent;
    }

    void setAuth(const char* auth) {
        ERA_TEMPLATABLE_COMPONENT_LOOP(setAuth, auth)
    }

    void setTopic(const char* topic) {
        ERA_TEMPLATABLE_COMPONENT_LOOP(setTopic, topic)
    }

    void onMessage(MessageCallback_t cb) {
        ERA_TEMPLATABLE_COMPONENT_LOOP(onMessage, cb)
    }

private:
    ERaComponent* mFirstComponent;
    ERaComponent* mLastComponent;
    bool mInitialized;
};

#endif /* INC_ERA_COMPONENT_HANDLER_HPP_ */
