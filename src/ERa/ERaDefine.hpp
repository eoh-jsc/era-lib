#ifndef INC_ERA_DEFINE_HPP_
#define INC_ERA_DEFINE_HPP_

#if !defined(ERA_MALLOC)
    #define ERA_MALLOC          era_malloc
#endif
#if !defined(ERA_REALLOC)
    #define ERA_REALLOC         era_realloc
#endif
#if !defined(ERA_CALLOC)
    #define ERA_CALLOC          era_calloc
#endif
#if !defined(ERA_FREE)
    #define ERA_FREE            era_free
#endif

#define ERA_NEWLINE             "\r\n"

#define ERA_CONCAT_2(x, y)      x ## y
#define ERA_CONCAT(x, y)        ERA_CONCAT_2(x, y)

#define ERA_STRINGIFY(x)        # x
#define ERA_TOSTRING(x)         ERA_STRINGIFY(x)

#define ERA_COUNT_OF(x)         ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define ERA_ATTR_PACKED         __attribute__((__packed__))
#define ERA_NORETURN            __attribute__((noreturn))
#define ERA_UNUSED              __attribute__((__unused__))
#define ERA_DEPRECATED          __attribute__((deprecated))
#define ERA_CONSTRUCTOR         __attribute__((constructor))
#define ERA_WEAK                __attribute__((weak))

#define ERA_FORCE_INLINE        inline //__attribute__((always_inline))

#define ERA_FORCE_UNUSED(x)     ((void)(x))

#if defined(ARDUINO) && defined(ESP32)
#if (ESP_IDF_VERSION_MAJOR < 4)
    #define SEND_UART(huart, command, length)   while(uart_write_bytes(huart, reinterpret_cast<const char*>(command), length) < 0)
#else
    #define SEND_UART(huart, command, length)   while(uart_write_bytes(huart, reinterpret_cast<const void*>(command), length) < 0)
#endif
    #define WAIT_SEND_UART_DONE(huart)          uart_wait_tx_done(huart, 5000)
    #define FLUSH_UART(huart)                   uart_flush(huart)

    #include <stddef.h>
    #include <esp32-hal-psram.h>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <stddef.h>
#else
    #include <stddef.h>
#endif

#include "../Utility/ERaLoc.hpp"

#endif /* INC_ERA_DEFINE_HPP_ */
