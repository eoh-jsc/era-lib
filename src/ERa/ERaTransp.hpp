#ifndef INC_ERA_TRANSP_HPP_
#define INC_ERA_TRANSP_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>

#if defined(__has_include) &&       \
    __has_include(<functional>) &&  \
    !defined(ERA_IGNORE_STD_FUNCTIONAL_STRING)
    #include <functional>
    #define TRANSP_HAS_FUNCTIONAL_H
#endif

class ERaTransp
    : public Client
{
protected:
#if defined(TRANSP_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaTransp()
    {}
    virtual ~ERaTransp()
    {}

    virtual void begin(void* args = NULL) = 0;
    virtual void run() = 0;
    virtual void setTopic(const char* topic) = 0;
    virtual void onMessage(MessageCallback_t cb) = 0;

protected:
};

#endif /* INC_ERA_TRANSP_HPP_ */
