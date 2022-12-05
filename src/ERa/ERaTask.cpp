#if defined(ARDUINO)
    #include <Arduino.h>
#endif
#include <ERa/ERaTask.hpp>
#include <ERa/ERaDefine.hpp>

ERA_WEAK void runERaLoopTask(void* args) {
#if defined(ARDUINO)
    for (;;) {
        loop();
    }
#endif
}
