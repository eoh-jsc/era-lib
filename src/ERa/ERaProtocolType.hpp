#ifndef INC_ERA_PROTOCOL_TYPE_HPP_
#define INC_ERA_PROTOCOL_TYPE_HPP_

#include <stdint.h>

enum class PublishIntervalKindT
    : uint8_t {
    ASK_TIMESTAMP = 0,
    INFO,
    SELF_INFO,
    COUNT
};

typedef struct __PublishIntervalState_t {
    __PublishIntervalState_t()
        : interval(0UL)
        , lastPublish(0UL)
    {}

    unsigned long interval;
    unsigned long lastPublish;
} PublishIntervalState_t;

#endif /* INC_ERA_PROTOCOL_TYPE_HPP_ */
