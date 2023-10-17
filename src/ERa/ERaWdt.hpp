#ifndef INC_ERA_WATCHDOG_HPP_
#define INC_ERA_WATCHDOG_HPP_

#include <stdint.h>
#include <ERa/ERaDefine.hpp>

#if defined(ERA_WATCHDOG_DISABLE)
    void ERaWatchdogEnable(unsigned long timeout) {
        ERA_FORCE_UNUSED(timeout);
    }

    void ERaWatchdogDisable() {
    }

    void ERaWatchdogFeed() {
    }
#endif

#endif /* INC_ERA_WATCHDOG_HPP_ */
