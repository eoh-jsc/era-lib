#ifndef INC_ERA_REASON_RESET_ESP32_HPP_
#define INC_ERA_REASON_RESET_ESP32_HPP_

#if (ESP_IDF_VERSION_MAJOR < 4)
    #include <rom/rtc.h>
    #define ERA_RESET_REASON
#else
    #if CONFIG_IDF_TARGET_ESP32
        #include <esp32/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32S2
        #include <esp32s2/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32C2
        #include <esp32c2/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32C3
        #include <esp32c3/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32S3
        #include <esp32s3/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32C6
        #include <esp32c6/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32H2
        #include <esp32h2/rom/rtc.h>
        #define ERA_RESET_REASON
    #endif
#endif

#if defined(ERA_RESET_REASON)
    static inline
    String SystemGetResetReason() {
        String rstReason;
        int reason = rtc_get_reset_reason(0);
        switch (reason) {
            case 1:
                rstReason = "POWERON_RESET";            /** <1,  Vbat power on reset */
                break;
            case 3:
                rstReason = "SW_RESET";                 /** <3,  Software reset digital core */
                break;
            case 4:
                rstReason = "OWDT_RESET";               /** <4,  Legacy watch dog reset digital core */
                break;
            case 5:
                rstReason = "DEEPSLEEP_RESET";          /** <5,  Deep Sleep reset digital core */
                break;
            case 6:
                rstReason = "SDIO_RESET";               /** <6,  Reset by SLC module, reset digital core */
                break;
            case 7:
                rstReason = "TG0WDT_SYS_RESET";         /** <7,  Timer Group0 Watch dog reset digital core */
                break;
            case 8:
                rstReason = "TG1WDT_SYS_RESET";         /** <8,  Timer Group1 Watch dog reset digital core */
                break;
            case 9:
                rstReason = "RTCWDT_SYS_RESET";         /** <9,  RTC Watch dog Reset digital core */
                break;
            case 10:
                rstReason = "INTRUSION_RESET";          /** <10, Instrusion tested to reset CPU */
                break;
            case 11:
                rstReason = "TGWDT_CPU_RESET";          /** <11, Time Group reset CPU */
                break;
            case 12:
                rstReason = "SW_CPU_RESET";             /** <12, Software reset CPU */
                break;
            case 13:
                rstReason = "RTCWDT_CPU_RESET";         /** <13, RTC Watch dog Reset CPU */
                break;
            case 14:
                rstReason = "EXT_CPU_RESET";            /** <14, for APP CPU, reseted by PRO CPU */
                break;
            case 15:
                rstReason = "RTCWDT_BROWN_OUT_RESET";   /** <15, Reset when the vdd voltage is not stable */
                break;
            case 16:
                rstReason = "RTCWDT_RTC_RESET";         /** <16, RTC Watch dog reset digital core and rtc module */
                break;
            case 17:
                rstReason = "TG1WDT_CPU_RESET";         /** <17, Time Group1 reset CPU */
                break;
            case 18:
                rstReason = "SUPER_WDT_RESET";          /** <18, Super watchdog reset digital core and rtc module */
                break;
            case 19:
                rstReason = "GLITCH_RTC_RESET";         /** <19, Glitch reset digital core and rtc module */
                break;
            case 20:
                rstReason = "EFUSE_RESET";              /** <20, Efuse reset digital core */
                break;
            case 21:
                rstReason = "USB_UART_CHIP_RESET";      /** <21, Usb uart reset digital core */
                break;
            case 22:
                rstReason = "USB_JTAG_CHIP_RESET";      /** <22, Usb jtag reset digital core */
                break;
            case 23:
                rstReason = "POWER_GLITCH_RESET";       /** <23, Power glitch reset digital core and rtc module */
                break;
            default:
                rstReason = "UNKNOWN";
                break;
        }
        return rstReason;
    }
#endif

#endif /* INC_ERA_REASON_RESET_ESP32_HPP_ */
