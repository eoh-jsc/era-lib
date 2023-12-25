#ifndef INC_ERA_DEBUG_HPP_
#define INC_ERA_DEBUG_HPP_

#if !defined(ERA_RUN_YIELD)
    #if !defined(ERA_RUN_YIELD_MS)
        #define ERA_RUN_YIELD_MS 0
    #endif
    #if !defined(ERA_NO_YIELD)
        #if defined(PARTICLE) || defined(SPARK)
            #define ERA_RUN_YIELD() { Particle.process(); }
        #else
            #define ERA_RUN_YIELD() { ERaDelay(ERA_RUN_YIELD_MS); }
        #endif
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
    #define vsnprintfp          vsnprintf_P
#else
    #define ERA_PROGMEM
    #define ERA_F(s)            s
    #define ERA_PSTR(s)         s
    #define ERA_FPSTR(s)        s
    #define printfp             printf
    #define vsnprintfp          vsnprintf
#endif

#if defined(ERA_DEBUG)
    #if defined(ERA_DEBUG_COLOR)
        #define ERA_LOG_RED                             "\033[0;31m"
        #define ERA_LOG_GREEN                           "\033[0;32m"
        #define ERA_LOG_YELLOW                          "\033[0;33m"
        #define ERA_LOG_RESET                           "\033[0m"
        #pragma message "Debugging with color support is only available in VSCode!!!"
    #else
        #define ERA_LOG_RED
        #define ERA_LOG_GREEN
        #define ERA_LOG_YELLOW
        #define ERA_LOG_RESET
    #endif

    #if defined(ARDUINO) && defined(ESP32) &&   \
        (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
        #define ERA_LOG_COLOR(COLOR, tag, format, ...)  ESP_LOGI(tag, ERA_LOG_ ##COLOR format ERA_LOG_RESET, ##__VA_ARGS__)
        #define ERA_LOG(tag, format, ...)               ERA_LOG_COLOR(GREEN, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_ERROR(tag, format, ...)         ERA_LOG_COLOR(RED, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_WARNING(tag, format, ...)       ERA_LOG_COLOR(YELLOW, tag, format, ##__VA_ARGS__)
    #elif defined(ARDUINO) &&                   \
        !defined(__MBED__) &&                   \
        (defined(ESP32) || defined(ESP8266) ||  \
        defined(ARDUINO_ARCH_STM32) ||          \
        defined(ARDUINO_ARCH_RP2040))
        #ifndef ERA_SERIAL
            #define ERA_SERIAL                          Serial
        #endif

        #define ERA_LOG_TIME()                          ERA_SERIAL.printfp(ERA_PSTR("[%6u]"), ERaMillis())
        #define ERA_LOG_TAG(tag)                        ERA_SERIAL.printfp(ERA_PSTR("[%s] "), tag)
        #define ERA_LOG_FN()                            ERA_SERIAL.printfp(ERA_PSTR("[%s:%d] %s(): "), ERaFileName(__FILE__), __LINE__, __FUNCTION__)
        #define ERA_LOG_COLOR(COLOR, tag, format, ...)  { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); ERA_SERIAL.printfp(ERA_LOG_ ##COLOR format ERA_LOG_RESET, ##__VA_ARGS__); ERA_SERIAL.println(); }
        #define ERA_LOG(tag, format, ...)               ERA_LOG_COLOR(GREEN, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_ERROR(tag, format, ...)         ERA_LOG_COLOR(RED, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_WARNING(tag, format, ...)       ERA_LOG_COLOR(YELLOW, tag, format, ##__VA_ARGS__)

        static inline
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
    #elif defined(ARDUINO) &&                   \
        (defined(__AVR__) ||                    \
        defined(__MBED__) ||                    \
        defined(RTL8722DM) ||                   \
        defined(ARDUINO_AMEBA) ||               \
        defined(ARDUINO_ARCH_AVR) ||            \
        defined(ARDUINO_ARCH_SAM) ||            \
        defined(ARDUINO_ARCH_SAMD) ||           \
        defined(ARDUINO_ARCH_RENESAS) ||        \
        defined(ARDUINO_ARCH_ARM) ||            \
        defined(ARDUINO_ARCH_ARC32))
        #ifndef ERA_SERIAL
            #define ERA_SERIAL                          Serial
        #endif

        #define ERA_LOG_TIME()                          ERaPrintf(ERA_PSTR("[%6u]"), ERaMillis())
        #define ERA_LOG_TAG(tag)                        ERaPrintf(ERA_PSTR("[%s] "), tag)
        #define ERA_LOG_FN()                            ERaPrintf(ERA_PSTR("[%s:%d] %s(): "), ERaFileName(__FILE__), __LINE__, __FUNCTION__)
        #define ERA_LOG_COLOR(COLOR, tag, format, ...)  { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); ERaPrintf(ERA_LOG_ ##COLOR format ERA_LOG_RESET, ##__VA_ARGS__); ERA_SERIAL.println(); }
        #define ERA_LOG(tag, format, ...)               ERA_LOG_COLOR(GREEN, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_ERROR(tag, format, ...)         ERA_LOG_COLOR(RED, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_WARNING(tag, format, ...)       ERA_LOG_COLOR(YELLOW, tag, format, ##__VA_ARGS__)

        static inline
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

        #include <stdio.h>
        #include <stdarg.h>

        static inline
        ERA_UNUSED size_t ERaPrintf(const char* ERA_PROGMEM format, ...) {
            va_list arg;
            va_list copy;
            va_start(arg, format);
            va_copy(copy, arg);
            char locBuf[128] {0};
            char* buf = locBuf;
            int len = vsnprintfp(buf, sizeof(locBuf), format, copy);
            va_end(copy);
            if (len < 0) {
                va_end(arg);
                return 0;
            }
            if (len >= (int)sizeof(locBuf)) {
                buf = (char*)malloc(len + 1);
                if (buf == nullptr) {
                    va_end(arg);
                    return 0;
                }
                len = vsnprintfp(buf, len + 1, format, arg);
            }
            va_end(arg);
            ERA_SERIAL.print(buf);
            if (buf != locBuf) {
                free(buf);
            }
            buf = nullptr;
            return len;
        }
    #elif defined(LINUX)
        #ifndef ERA_SERIAL
            #define ERA_SERIAL                          stdout
        #endif

        #if defined(RASPBERRY) ||       \
            defined(TINKER_BOARD) ||    \
            defined(ORANGE_PI)
            #include <wiringPi.h>
        #endif

        #include <iostream>
        using namespace std;
        #define ERA_LOG_TIME()                          cout << '[' << ERaMillis() << ']'
        #define ERA_LOG_TAG(tag)                        cout << '[' << tag << "] "
        #define ERA_LOG_FN()                            cout << '[' << ERaFileName(__FILE__) << ':' << __LINE__ << "] " << __FUNCTION__ << "(): "
        #define ERA_LOG_COLOR(COLOR, tag, format, ...)  { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); fprintf(ERA_SERIAL, ERA_LOG_ ##COLOR format ERA_LOG_RESET ERA_NEWLINE, ##__VA_ARGS__); }
        #define ERA_LOG(tag, format, ...)               ERA_LOG_COLOR(GREEN, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_ERROR(tag, format, ...)         ERA_LOG_COLOR(RED, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_WARNING(tag, format, ...)       ERA_LOG_COLOR(YELLOW, tag, format, ##__VA_ARGS__)

        static inline
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
            #define ERA_SERIAL                          Serial
        #endif

        #define ERA_LOG_TIME()                          ERA_SERIAL.printfp(ERA_PSTR("[%6u]"), ERaMillis())
        #define ERA_LOG_TAG(tag)                        ERA_SERIAL.printfp(ERA_PSTR("[%s] "), tag)
        #define ERA_LOG_FN()                            ERA_SERIAL.printfp(ERA_PSTR("[%s:%d] %s(): "), ERaFileName(__FILE__), __LINE__, __FUNCTION__)
        #define ERA_LOG_COLOR(COLOR, tag, format, ...)  { ERA_LOG_TIME(); ERA_LOG_FN(); ERA_LOG_TAG(tag); ERA_SERIAL.printfp(ERA_LOG_ ##COLOR format ERA_LOG_RESET, ##__VA_ARGS__); ERA_SERIAL.println(); }
        #define ERA_LOG(tag, format, ...)               ERA_LOG_COLOR(GREEN, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_ERROR(tag, format, ...)         ERA_LOG_COLOR(RED, tag, format, ##__VA_ARGS__)
        #define ERA_LOG_WARNING(tag, format, ...)       ERA_LOG_COLOR(YELLOW, tag, format, ##__VA_ARGS__)

        static inline
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

    #if defined(ERA_SERIAL) &&              \
        (defined(ARDUINO) || defined(LINUX))
        #include <ERa/ERaDebugHelper.hpp>
        #define ERA_PRINT(...)                          LogHelper::instance().print(__VA_ARGS__)
    #else
        #define ERA_PRINT(...)                          do {} while(0)
    #endif

    #pragma message "Debug enabled"
    #if defined(ARDUINO) && defined(ESP32)
        #if (CORE_DEBUG_LEVEL < ARDUHAL_LOG_LEVEL_INFO)
            #pragma message "Recommend: Setting the Core Debug Level to Info (3)"
        #endif
        #if defined(BOARD_HAS_PSRAM)
            #pragma message "PSRAM enabled"
        #endif
    #endif
#else
    #undef ERA_LOG
    #undef ERA_LOG_ERROR
    #undef ERA_LOG_WARNING
    #undef ERA_PRINT
    #undef ERA_DEBUG_DUMP
    #define ERA_LOG(...)            do {} while(0)
    #define ERA_LOG_ERROR(...)      do {} while(0)
    #define ERA_LOG_WARNING(...)    do {} while(0)
    #define ERA_PRINT(...)          do {} while(0)
#endif

#ifndef ERA_NO_ASSERT
    #include <assert.h>
    #define ERA_ASSERT(expr)        assert(expr)
#else
    #define ERA_ASSERT(expr)
#endif

#define ERA_ASSERT_NULL(data, rt)   \
    if (data == nullptr) {          \
        ERA_ASSERT(data != nullptr);\
        return rt;                  \
    }

static inline
ERA_UNUSED void ERaLogHex(const char ERA_UNUSED *title, const uint8_t ERA_UNUSED *buf, size_t ERA_UNUSED len) {
#ifdef ERA_DEBUG_DUMP
    if (title == nullptr) {
        return;
    }
    if (buf == nullptr) {
        return;
    }
    size_t size {strlen(title)};
    size += (2 + 8 * sizeof(len));
    size += (3 * len);
    size += 10;
    char locBuf[128] {0};
    char* out = locBuf;
    if (size > sizeof(locBuf)) {
        out = (char*)malloc(size);
        if (out == nullptr) {
            return;
        }
        memset(out, 0, size);
    }
    snprintf(out + strlen(out), size - strlen(out), title);
    snprintf(out + strlen(out), size - strlen(out), " (");
    snprintf(out + strlen(out), size - strlen(out), "%3d", len);
    snprintf(out + strlen(out), size - strlen(out), "): ");
    for (size_t i = 0; i < len; ++i) {
        snprintf(out + strlen(out), size - strlen(out), "%02X ", buf[i]);
    }
    ERA_LOG(ERA_PSTR("Hex"), ERA_PSTR("%s"), out);
    if (out != locBuf) {
        free(out);
    }
    out = nullptr;
#endif
}

#endif /* INC_ERA_DEBUG_HPP_ */
