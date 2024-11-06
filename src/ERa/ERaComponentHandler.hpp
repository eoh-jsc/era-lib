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
    {}
    ~ERaComponentHandler()
    {}

protected:
    void begin() {
        ERA_TEMPLATABLE_COMPONENT_LOOP(begin);
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
};

#endif /* INC_ERA_COMPONENT_HANDLER_HPP_ */
