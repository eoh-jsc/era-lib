#ifndef INC_ERA_TIMESTAMP_TYPE_HPP_
#define INC_ERA_TIMESTAMP_TYPE_HPP_

#include <ERa/ERaTimeLib.hpp>

enum class TimestampKindT
    : uint8_t {
    CONFIG_VALUE = 0,
#if defined(ERA_MODBUS)
    MODBUS,
#endif
#if defined(ERA_ZIGBEE)
    ZIGBEE,
#endif
    OTHER,
    COUNT
};

typedef struct __TimestampState_t {
    __TimestampState_t()
        : msAnchor(0UL)
        , lastSec(0UL)
        , lastTimestamp(0.0)
    {}

    MillisTime_t msAnchor;
    ERaTime::time_type_t lastSec;
    double lastTimestamp;
} TimestampState_t;

#endif /* INC_ERA_TIMESTAMP_TYPE_HPP_ */
