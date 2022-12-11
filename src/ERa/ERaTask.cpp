#if defined(ARDUINO)
    #include <Arduino.h>
#endif
#include <ERa/ERaTask.hpp>

ERA_WEAK void runERaLoopTask(void ERA_UNUSED *args) {
#if defined(ARDUINO)
    for (;;) {
        loop();
    }
#endif
}
