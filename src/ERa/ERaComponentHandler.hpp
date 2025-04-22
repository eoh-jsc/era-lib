#ifndef INC_ERA_COMPONENT_HANDLER_HPP_
#define INC_ERA_COMPONENT_HANDLER_HPP_

#include <ERa/ERaComponent.hpp>

#define ERA_TEMPLATABLE_COMPONENT_LOOP(method, ...) \
    ERaComponent* component = this->firstComponent; \
    while (component != NULL) {                     \
        ERaComponent* next = component->getNext();  \
        component->method(__VA_ARGS__);             \
        component = next;                           \
    }

class ERaComponentHandler
{
public:
    ERaComponentHandler()
        : firstComponent(NULL)
        , lastComponent(NULL)
        , initialized(false)
    {}
    ~ERaComponentHandler()
    {}

protected:
    void begin() {
        if (this->initialized) {
            return;
        }
        ERA_TEMPLATABLE_COMPONENT_LOOP(begin);
        this->initialized = true;
    }

    void run() {
        ERA_TEMPLATABLE_COMPONENT_LOOP(run);
    }

    void addComponent(ERaComponent& component) {
        this->addComponent(&component);
    }

    void addComponent(ERaComponent* pComponent) {
        if (this->lastComponent == NULL) {
            this->firstComponent = pComponent;
        }
        else {
            this->lastComponent->setNext(pComponent);
        }
        this->lastComponent = pComponent;
    }

private:
    ERaComponent* firstComponent;
    ERaComponent* lastComponent;
    bool initialized;
};

#endif /* INC_ERA_COMPONENT_HANDLER_HPP_ */
