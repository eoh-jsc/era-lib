#ifndef INC_ERA_COMPONENT_HPP_
#define INC_ERA_COMPONENT_HPP_

#include <stdint.h>
#include <stddef.h>

class ERaComponent
{
public:
    ERaComponent()
        : next(NULL)
    {}
    virtual ~ERaComponent()
    {}

    virtual void begin() = 0;
    virtual void run() = 0;

    ERaComponent* getNext() const {
        return this->next;
    }

    void setNext(ERaComponent& component) {
        this->next = &component;
    }

    void setNext(ERaComponent* component) {
        this->next = component;
    }

protected:
private:
    ERaComponent* next;
};

#endif /* INC_ERA_COMPONENT_HPP_ */
