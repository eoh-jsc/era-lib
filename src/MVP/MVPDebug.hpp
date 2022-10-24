#ifndef INC_MVP_DEBUG_HPP_
#define INC_MVP_DEBUG_HPP_

#if defined(__AVR__)
    #include <avr/pgmspace.h>
    #define MVP_PROGMEM         PROGMEM
    #define MVP_F(s)            F(s)
    #define MVP_PSTR(s)         PSTR(s)
#else
    #define MVP_PROGMEM
    #define MVP_F(s)            s
    #define MVP_PSTR(s)         s
#endif

#if defined(ARDUINO) && defined(ESP32)
    #define MVP_LOG(tag, ...)           ESP_LOGI(tag, ##__VA_ARGS__)
#elif defined(ARDUINO) && defined(ESP8266)
    #ifndef MVP_SERIAL
        #define MVP_SERIAL              Serial
    #endif

    #define MVP_LOG_TIME()              MVP_SERIAL.printf("[%6u]", MVPMillis())
    #define MVP_LOG_TAG(tag)            MVP_SERIAL.printf("[%s] ", tag)
    #define MVP_LOG_FN()                MVP_SERIAL.printf("[%s:%d] %s(): ", MVPFileName(__FILE__), __LINE__, __FUNCTION__)
    #define MVP_LOG(tag, format, ...)   { MVP_LOG_TIME(); MVP_LOG_FN(); MVP_LOG_TAG(tag); MVP_SERIAL.printf(format MVP_NEWLINE, ##__VA_ARGS__); }
    
    static
    MVP_UNUSED const char* MVPFileName(const char* path) {
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
    #ifndef MVP_SERIAL
        #define MVP_SERIAL              stdout
    #endif

    #include <iostream>
    using namespace std;
    #define MVP_LOG_TIME()              cout << '[' << MVPMillis() << ']'
    #define MVP_LOG_TAG(tag)            cout << '[' << tag << "] "
    #define MVP_LOG_FN()                cout << '[' << MVPFileName(__FILE__) << ':' << __LINE__ << "] " << __FUNCTION__ << "(): "
    #define MVP_LOG(tag, format, ...)   { MVP_LOG_TIME(); MVP_LOG_FN(); MVP_LOG_TAG(tag); fprintf(MVP_SERIAL, format MVP_NEWLINE, ##__VA_ARGS__); }

    static
    MVP_UNUSED const char* MVPFileName(const char* path) {
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
    #ifndef MVP_SERIAL
        #define MVP_SERIAL              Serial
    #endif

    #define MVP_LOG_TIME()              MVP_SERIAL.printf("[%6u]", MVPMillis())
    #define MVP_LOG_TAG(tag)            MVP_SERIAL.printf("[%s] ", tag)
    #define MVP_LOG_FN()                MVP_SERIAL.printf("[%s:%d] %s(): ", MVPFileName(__FILE__), __LINE__, __FUNCTION__)
    #define MVP_LOG(tag, format, ...)   { MVP_LOG_TIME(); MVP_LOG_FN(); MVP_LOG_TAG(tag); MVP_SERIAL.printf(format MVP_NEWLINE, ##__VA_ARGS__); }

    static
    MVP_UNUSED const char* MVPFileName(const char* path) {
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

#ifdef MVP_DEBUG
    #pragma message "Debug enabled"
#else
    #undef MVP_LOG
    #define MVP_LOG(...)
#endif

static
MVP_UNUSED void MVPLogHex(const char* title, const uint8_t* buf, size_t len) {
#ifdef MVP_DEBUG
    char str[5] {0};
    std::string out(title);
    snprintf(str, sizeof(str), "%3d", len);
    out.append("(");
    out.append(str);
    out.append("): ");
    for (size_t i = 0; i < len; ++i) {
        snprintf(str, sizeof(str), "%02X ", buf[i]);
        out.append(str);
    }
    MVP_LOG("Hex", "%s", out.c_str());
#endif
}

#endif /* INC_MVP_DEBUG_HPP_ */
