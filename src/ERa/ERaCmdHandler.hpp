#ifndef INC_ERA_COMMAND_HANDLER_HPP_
#define INC_ERA_COMMAND_HANDLER_HPP_

#include <ERa/ERaDetect.hpp>
#include <Utility/ERacJSON.hpp>

#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> HandlerSimple_t;
#else
    typedef void (*HandlerSimple_t)(void);
#endif

class ERaCmdHandler
{
public:
    const char* cmd;
    HandlerSimple_t fn;

    ERaCmdHandler() = default;
    ERaCmdHandler(const char* _cmd, HandlerSimple_t _fn)
        : cmd(_cmd)
        , fn(_fn)
        , next(nullptr)
    {}

    ERaCmdHandler* getNext() const {
        return this->next;
    }

    void setNext(ERaCmdHandler* _next) {
        this->next = _next;
    }

private:
    ERaCmdHandler* next;
};

#endif /* INC_ERA_COMMAND_HANDLER_HPP_ */
