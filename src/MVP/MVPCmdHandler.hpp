#ifndef INC_MVP_COMMAND_HANDLER_HPP_
#define INC_MVP_COMMAND_HANDLER_HPP_

#include <functional>
#include <Utility/MVPcJSON.hpp>

typedef std::function<void(void)> HandlerSimple_t;

class MVPCmdHandler
{
public:
    const char* cmd;
    HandlerSimple_t fn;

    MVPCmdHandler() = default;
    MVPCmdHandler(const char* _cmd, HandlerSimple_t _fn)
        : cmd(_cmd)
        , fn(_fn)
        , next(nullptr)
    {}

    MVPCmdHandler* getNext() {
        return this->next;
    }

    void setNext(MVPCmdHandler* _next) {
        this->next = _next;
    }

private:
    MVPCmdHandler* next;
};

#endif /* INC_MVP_COMMAND_HANDLER_HPP_ */