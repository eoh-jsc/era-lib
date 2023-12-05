#ifndef INC_ERA_REASON_RESET_HPP_
#define INC_ERA_REASON_RESET_HPP_

#if defined(ESP32)
    #include <Reason/ERaReasonEsp32.hpp>
#elif defined(ESP8266)
    #include <Reason/ERaReasonEsp8266.hpp>
#elif defined(ARDUINO_ARCH_STM32)
    #include <Reason/ERaReasonStm32.hpp>
#endif

#if defined(ARDUINO)
    #if !defined(ERA_RESET_REASON)
        #define ERA_RESET_REASON

        static inline
        String SystemGetResetReason() {
            return String("UNKNOWN");
        }
    #endif
#endif

#endif /* INC_ERA_REASON_RESET_HPP_ */
