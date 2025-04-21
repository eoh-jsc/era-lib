#ifndef INC_ERA_PIN_DEFINE_HPP_
#define INC_ERA_PIN_DEFINE_HPP_

#include <stdint.h>

#if defined(ERA_REPORTABLE_CHANGE_PIN)
    /* OK, use the specified value */
#else
    #define ERA_REPORTABLE_CHANGE_PIN   10.0f
#endif

#if defined(ERA_SETTING_REPORT_PIN)
    /* OK, use the specified value */
#else
    #define ERA_SETTING_REPORT_PIN      1000, 1000, 60000, ERA_REPORTABLE_CHANGE_PIN
#endif

typedef struct __PinConfig_t {
    uint16_t pin;
    uint8_t pinMode;
    ERaUInt_t configId;
    struct __PWMConfig_t {
        __PWMConfig_t()
            : channel(0)
            , resolution(8)
            , frequency(1000)
        {}
        int8_t channel;
        uint8_t resolution;
        uint32_t frequency;
    } pwm;
    struct __ReportConfig_t {
        __ReportConfig_t()
            : interval(100UL)
            , minInterval(100UL)
            , maxInterval(60000UL)
            , reportableChange(1.0f)
        {}
        __ReportConfig_t(unsigned long _interval, unsigned long _minInterval,
                        unsigned long _maxInterval, float _reportableChange)
            : interval(_interval)
            , minInterval(_minInterval)
            , maxInterval(_maxInterval)
            , reportableChange(_reportableChange)
        {}
        unsigned long interval;
        unsigned long minInterval;
        unsigned long maxInterval;
        float reportableChange;
    } report;
    struct __ScaleConfig_t {
        float min;
        float max;
        float rawMin;
        float rawMax;
    } scale;
} PinConfig_t;

#endif /* INC_ERA_PIN_DEFINE_HPP_ */
