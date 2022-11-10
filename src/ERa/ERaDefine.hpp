#ifndef INC_ERA_DEFINE_HPP_
#define INC_ERA_DEFINE_HPP_

#if !defined(ERA_MALLOC)
    #define ERA_MALLOC          malloc
#endif
#if !defined(ERA_REALLOC)
    #define ERA_REALLOC         realloc
#endif
#if !defined(ERA_CALLOC)
    #define ERA_CALLOC          calloc
#endif

#define ERA_NEWLINE             "\r\n"

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
    #define SEND_UART(huart, command, length)   while(uart_write_bytes(huart, reinterpret_cast<char*>(command), length) < 0)
    #define WAIT_SEND_UART_DONE(huart)          uart_wait_tx_done(huart, 5000)
    #define FLUSH_UART(huart)                   uart_flush(huart)

    #include <stddef.h>
    #include <esp32-hal-psram.h>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <stddef.h>
#else
    #include <stddef.h>
#endif

#endif /* INC_ERA_DEFINE_HPP_ */
