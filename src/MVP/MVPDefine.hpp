#ifndef INC_MVP_DEFINE_HPP_
#define INC_MVP_DEFINE_HPP_

#define MVP_MALLOC              malloc
#define MVP_REALLOC             realloc
#define MVP_CALLOC              calloc

#define MVP_NEWLINE             "\r\n"

#define MVP_STRINGIFY(x)        # x
#define MVP_TOSTRING(x)         MVP_STRINGIFY(x)

#define MVP_COUNT_OF(x)         ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define MVP_ATTR_PACKED         __attribute__((__packed__))
#define MVP_NORETURN            __attribute__((noreturn))
#define MVP_UNUSED              __attribute__((__unused__))
#define MVP_DEPRECATED          __attribute__((deprecated))
#define MVP_CONSTRUCTOR         __attribute__((constructor))
#define MVP_WEAK                __attribute__((weak))

#define MVP_FORCE_INLINE        inline //__attribute__((always_inline))

#define UNUSED(x)               ((void)(x))

#define TOGGLE                  0x2
#define PWM                     0x27

#if defined(ARDUINO) && defined(ESP32)
    #define MVP_MODBUS

    #define SEND_UART(huart, command, length)   while(uart_write_bytes(huart, reinterpret_cast<char*>(command), length) < 0)
    #define WAIT_SEND_UART_DONE(huart)          uart_wait_tx_done(huart, 5000)

    #include <stddef.h>
    #include <esp32-hal-psram.h>
#endif

#endif /* INC_MVP_DEFINE_HPP_ */