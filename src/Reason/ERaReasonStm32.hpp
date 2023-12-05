#ifndef INC_ERA_REASON_RESET_STM32_HPP_
#define INC_ERA_REASON_RESET_STM32_HPP_

#if defined(STM32F4xx) ||   \
    defined(STM32F7xx)
    #include "stm32yyxx_ll_utils.h"
    #define ERA_RESET_REASON
#endif

#if defined(ERA_RESET_REASON)
    static inline
    uint8_t SystemGetFlagReason() {
        static uint8_t reason {0xFF};
        if (reason != 0xFF) {
            return reason;
        }
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
            reason = 0;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
            reason = 1;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
            reason = 2;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
            reason = 3;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
            reason = 4;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
            reason = 5;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)) {
            reason = 6;
        }
        else {
            reason = 0xFE;
        }
        __HAL_RCC_CLEAR_RESET_FLAGS();
        return reason;
    }

    static inline
    String SystemGetResetReason() {
        String rstReason;
        int reason = SystemGetFlagReason();
        switch (reason) {
            case 0:
                rstReason = "LOW_POWER_RESET";
                break;
            case 1:
                rstReason = "WINDOW_WATCHDOG_RESET";
                break;
            case 2:
                rstReason = "INDEPENDENT_WATCHDOG_RESET";
                break;
            case 3:
                rstReason = "SOFTWARE_RESET";
                break;
            case 4:
                rstReason = "POWER_ON_RESET";
                break;
            case 5:
                rstReason = "EXTERNAL_PIN_RESET";
                break;
            case 6:
                rstReason = "BROWNOUT_RESET";
                break;
            default:
                rstReason = "UNKNOWN";
                break;
        }
        return rstReason;
    }
#endif

#endif /* INC_ERA_REASON_RESET_STM32_HPP_ */
