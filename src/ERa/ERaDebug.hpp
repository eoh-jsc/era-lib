#ifndef INC_ERA_DEBUG_HPP_
#define INC_ERA_DEBUG_HPP_

#if !defined(ERA_RUN_YIELD)
    #if !defined(ERA_RUN_YIELD_MS)
        #define ERA_RUN_YIELD_MS 0
    #endif
    #if !defined(ERA_NO_YIELD)
        #define ERA_RUN_YIELD() { ERaDelay(ERA_RUN_YIELD_MS); }
    #else
        #define ERA_RUN_YIELD() {}
    #endif
#endif

#if defined(__AVR__) || \
    (defined(ARDUINO) && defined(ESP8266))
    #include <avr/pgmspace.h>
    #define ERA_HAS_PROGMEM
    #define ERA_PROGMEM         PROGMEM
    #define ERA_F(s)            F(s)
    #define ERA_PSTR(s)         PSTR(s)
    #define ERA_FPSTR(s)        FPSTR(s)
    #define printfp             printf_P
#else
    #define ERA_PROGMEM
    #define ERA_F(s)            s
    #define ERA_PSTR(s)         s
    #define ERA_FPSTR(s)        s
    #define printfp             printf
#endif

#if defined(ERA_DEBUG)
    #if defined(ARDUINO) && defined(ESP32) && \
        (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
        #define ERA_LOG(tag, ...)           ESP_LOGI(tag, ##__VA_ARGS__)
    #elif defined(ARDUINO) &&                                                   \
        (defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040) ||  \
        defined(STM32F0xx) || defined(STM32F1xx) || defined(STM32F2xx) ||       \
        defined(STM32F3xx) || defined(STM32F4xx) || defined(STM32F7xx))
        #ifndef ERA_SERIAL
            #define ERA_SERIAL              Serial
        #endif

        #define ERA_LOG_TIME()              ERA_SERIAL.printfp(ERA_PSTR("[%6u]"), ERaMillis())
        #define ERA_LOG_TAG(tag)            ERA_SERIAL.printfp(ERA_PSTR("[%s] "), tag)
        #define ERA_LOG_FN()                ERA_SERIAL.printfp(ERA_PSTR("[%s:%d] %s(): "), ERaFileName(__FILE__), __LINE__, __FUNCTION__)
        #define ERA_LOG(tag, format, ...)   { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); ERA_SERIAL.printfp(format, ##__VA_ARGS__); ERA_SERIAL.println(); }
        
        static
        ERA_UNUSED const char* ERaFileName(const char* path) {
            size_t i = 0;
            size_t pos = 0;
            char* p = (char*)path;
            while (*p) {
                i++;
                if(*p == '/' || *p == '\\'){
                    pos = i;
                }
                p++;
            }
            return path + pos;
        }
    #elif defined(LINUX)
        #ifndef ERA_SERIAL
            #define ERA_SERIAL              stdout
        #endif

        #if defined(RASPBERRY)
            #include <wiringPi.h>
        #endif

        #include <iostream>
        using namespace std;
        #define ERA_LOG_TIME()              cout << '[' << ERaMillis() << ']'
        #define ERA_LOG_TAG(tag)            cout << '[' << tag << "] "
        #define ERA_LOG_FN()                cout << '[' << ERaFileName(__FILE__) << ':' << __LINE__ << "] " << __FUNCTION__ << "(): "
        #define ERA_LOG(tag, format, ...)   { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); fprintf(ERA_SERIAL, format ERA_NEWLINE, ##__VA_ARGS__); }

        static
        ERA_UNUSED const char* ERaFileName(const char* path) {
            size_t i = 0;
            size_t pos = 0;
            char* p = (char*)path;
            while (*p) {
                i++;
                if(*p == '/' || *p == '\\'){
                    pos = i;
                }
                p++;
            }
            return path + pos;
        }
    #else
        #ifndef ERA_SERIAL
            #define ERA_SERIAL              Serial
        #endif

        #define ERA_LOG_TIME()              ERA_SERIAL.printfp(ERA_PSTR("[%6u]"), ERaMillis())
        #define ERA_LOG_TAG(tag)            ERA_SERIAL.printfp(ERA_PSTR("[%s] "), tag)
        #define ERA_LOG_FN()                ERA_SERIAL.printfp(ERA_PSTR("[%s:%d] %s(): "), ERaFileName(__FILE__), __LINE__, __FUNCTION__)
        #define ERA_LOG(tag, format, ...)   { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); ERA_SERIAL.printfp(format, ##__VA_ARGS__); ERA_SERIAL.println(); }

        static
        ERA_UNUSED const char* ERaFileName(const char* path) {
            size_t i = 0;
            size_t pos = 0;
            char* p = (char*)path;
            while (*p) {
                i++;
                if(*p == '/' || *p == '\\'){
                    pos = i;
                }
                p++;
            }
            return path + pos;
        }
    #endif

    #pragma message "Debug enabled"
    #if defined(ARDUINO) && defined(ESP32) && \
        (CORE_DEBUG_LEVEL < ARDUHAL_LOG_LEVEL_INFO)
        #pragma message "Recommend: Set Core Debug Level to Info(3)"
    #endif
#else
    #undef ERA_LOG
    #undef ERA_DEBUG_DUMP
    #define ERA_LOG(...)
#endif

static
ERA_UNUSED void ERaLogHex(const char ERA_UNUSED *title, const uint8_t ERA_UNUSED *buf, size_t ERA_UNUSED len) {
#ifdef ERA_DEBUG_DUMP
    char str[5] {0};
    std::string out(title);
    snprintf(str, sizeof(str), "%3d", len);
    out.append(" (");
    out.append(str);
    out.append("): ");
    for (size_t i = 0; i < len; ++i) {
        snprintf(str, sizeof(str), "%02X ", buf[i]);
        out.append(str);
    }
    ERA_LOG(ERA_PSTR("Hex"), ERA_PSTR("%s"), out.c_str());
#endif
}

#endif /* INC_ERA_DEBUG_HPP_ */
