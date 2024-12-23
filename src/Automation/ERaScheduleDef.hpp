#pragma once

#define ERA_SCHEDULE_UNIT_MASK    0xC0000000
#define ERA_SCHEDULE_UNIT_SHIFT   30

#define ERA_SCHEDULE_TYPE_MASK    0x3C000000
#define ERA_SCHEDULE_TYPE_SHIFT   26

#define ERA_SCHEDULE_MONTH_MASK   0x0000FF00
#define ERA_SCHEDULE_MONTH_SHIFT  8

#define ERA_SCHEDULE_REP_MASK     0x03FFFFFF
#define ERA_SCHEDULE_WEEK_MASK    0x000000FF
#define ERA_SCHEDULE_DAY_MASK     0x000000FF

#define ERA_SCHEDULE_ONE_SHOT     0xFFFFFFFF

namespace eras {

    enum class ScheduleUnit
        : int {
        Seconds      = 0,
        Minutes      = 1,
        Hours        = 2,
        Days         = 3
    };

    enum class ScheduleType
        : int {
        OneShot      = 0,
        FixedDelta   = 1,
        Weekly       = 2,
        Monthly      = 3,
        Yearly       = 4
    };

    enum class ScheduleMonth
        : int {
        Jan          = 0,
        Feb          = 1,
        Mar          = 2,
        Apr          = 3,
        May          = 4,
        Jun          = 5,
        Jul          = 6,
        Aug          = 7,
        Sep          = 8,
        Oct          = 9,
        Nov          = 10,
        Dec          = 11
    };

    enum class ScheduleWeekDay
        : int {
        Sun          = 0,
        Mon          = 1,
        Tue          = 2,
        Wed          = 3,
        Thu          = 4,
        Fri          = 5,
        Sat          = 6
    };

    enum class ScheduleState
        : int {
        Inactive     = 0,
        Active       = 1
    };

    typedef struct ScheduleWeeklyMask {
        ScheduleState& operator[](ScheduleWeekDay i) {
            return day[static_cast<int>(i) % 7];
        }
        ScheduleState day[7];
    } ScheduleWeeklyMask;

    typedef unsigned int ScheduleTimeType;
    typedef unsigned int ScheduleConfigurationType;

} /* namespace eras */
