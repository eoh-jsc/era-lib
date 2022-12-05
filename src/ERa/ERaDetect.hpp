#ifndef INC_ERA_DETECT_HPP_
#define INC_ERA_DETECT_HPP_

#if defined(ARDUINO) && defined(ESP32)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    5120
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #define ERA_100_PINS

    #if defined(CONFIG_FREERTOS_UNICORE)
        #define ERA_MCU_CORE            0
    #else
        #define ERA_MCU_CORE            1
    #endif

    #if defined(ARDUINO_ESP32C3_DEV) || \
        defined(ARDUINO_ESP32S2_DEV) || \
        defined(ARDUINO_ESP32S3_DEV)
        #define ERA_CHIP_TEMP
    #elif defined(ARDUINO_ARCH_ESP32)
        #define ERA_CHIP_TEMP_DEPRECATED
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP32C3_DEV)
            #define ERA_BOARD_TYPE      "ESP32C3"
        #elif defined(ARDUINO_ESP32S2_DEV)
            #define ERA_BOARD_TYPE      "ESP32S2"
        #elif defined(ARDUINO_ESP32S3_DEV)
            #define ERA_BOARD_TYPE      "ESP32S3"
        #elif defined(ARDUINO_ESP32_S3_BOX)
            #define ERA_BOARD_TYPE      "ESP32S3-BOX"
        #elif defined(ARDUINO_ARCH_ESP32)
            #define ERA_BOARD_TYPE      "ESP32"
        #else
            #define ERA_BOARD_TYPE      "ESP32"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ESP8266)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         512
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP8266_NODEMCU)
            #define ERA_BOARD_TYPE      "NodeMCU"
        #elif defined(ARDUINO_ARCH_ESP8266)
            #define ERA_BOARD_TYPE      "ESP8266"
        #else
            #define ERA_BOARD_TYPE      "ESP8266"
        #endif
    #endif

#elif defined(ARDUINO) && defined(STM32F4xx)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    5120
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #define ERA_100_PINS

    #if defined(ERA_NO_RTOS)
        #define ERA_NO_YIELD
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "STM32F4"
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_RP2040)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    5120
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #define ERA_100_PINS

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "RP2040"
    #endif

#elif defined(ARDUINO)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "Arduino"
    #endif

#elif defined(__MBED__)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         512
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #define ERA_100_PINS

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "MBED"
    #endif

    #define noInterrupts()              __disable_irq()
    #define interrupts()                __enable_irq()

#elif defined(LINUX)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    10240
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #define ERA_100_PINS

    #if !defined(ERA_BOARD_TYPE)
        #if defined(RASPBERRY)
            #define ERA_BOARD_TYPE      "Raspberry"
        #else
            #define ERA_BOARD_TYPE      "Linux"
        #endif
    #endif

#else

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "Custom"
    #endif

#endif

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE              "WiFi"
#endif

#endif /* INC_ERA_DETECT_HPP_ */
