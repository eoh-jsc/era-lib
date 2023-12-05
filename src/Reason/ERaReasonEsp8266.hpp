#ifndef INC_ERA_REASON_RESET_ESP8266_HPP_
#define INC_ERA_REASON_RESET_ESP8266_HPP_

extern "C" {
    #include <user_interface.h>
}

#define ERA_RESET_REASON

#if defined(ERA_RESET_REASON)
    static inline
    String SystemGetResetReason() {
        struct rst_info* rtcInfo = system_get_rst_info();
        if (rtcInfo == NULL) {
            return "UNKNOWN";
        }
        String rstReason;
        switch (rtcInfo->reason) {
            case 0:
                rstReason = "REASON_DEFAULT_RST";       /** <0, Normal startup by power on */
                break;
            case 1:
                rstReason = "REASON_WDT_RST";           /** <1, Hardware watch dog reset */
                break;
            case 2:
                rstReason = "REASON_EXCEPTION_RST";     /** <2, Exception reset, GPIO status won’t change */
                break;
            case 3:
                rstReason = "REASON_SOFT_WDT_RST";      /** <3, Software watch dog reset, GPIO status won’t change */
                break;
            case 4:
                rstReason = "REASON_SOFT_RESTART";      /** <4, Software restart ,system_restart , GPIO status won’t change */
                break;
            case 5:
                rstReason = "REASON_DEEP_SLEEP_AWAKE";  /** <5, Wake up from deep-sleep */
                break;
            case 6:
                rstReason = "REASON_EXT_SYS_RST";       /** <6, External system reset */
                break;
            default:
                rstReason = "UNKNOWN";
                break;
        }
        return rstReason;
    }
#endif

#endif /* INC_ERA_REASON_RESET_ESP8266_HPP_ */
