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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #define ERA_100_PINS

    #if defined(CONFIG_FREERTOS_UNICORE) || \
        (portNUM_PROCESSORS == 1)
        #define ERA_MCU_CORE            0
    #else
        #define ERA_MCU_CORE            1
    #endif

    #if defined(ARDUINO_ESP32C3_DEV) ||                                 \
        defined(ARDUINO_ADAFRUIT_QTPY_ESP32C3) ||                       \
        defined(ARDUINO_AirM2M_CORE_ESP32C3) ||                         \
        defined(ARDUINO_LOLIN_C3_MINI) ||                               \
        defined(ARDUINO_XIAO_ESP32C3) ||                                \
        defined(ARDUINO_WiFiduinoV2) ||                                 \
        defined(ARDUINO_TTGO_T_OI_PLUS_DEV) ||                          \
        defined(ARDUINO_ESP32S2_DEV) ||                                 \
        defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT) ||                \
        defined(ARDUINO_FUNHOUSE_ESP32S2) ||                            \
        defined(ARDUINO_MAGTAG29_ESP32S2) ||                            \
        defined(ARDUINO_METRO_ESP32S2) ||                               \
        defined(ARDUINO_ADAFRUIT_QTPY_ESP32S2) ||                       \
        defined(ARDUINO_atmegazero_esp32s2) ||                          \
        defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM) ||            \
        defined(ARDUINO_FRANZININHO_WIFI) ||                            \
        defined(ARDUINO_FRANZININHO_WIFI_MSC) ||                        \
        defined(ARDUINO_LOLIN_S2_MINI) ||                               \
        defined(ARDUINO_LOLIN_S2_PICO) ||                               \
        defined(ARDUINO_MICROS2) ||                                     \
        defined(ARDUINO_DEPARTMENT_OF_ALCHEMY_MINIMAIN_ESP32S2) ||      \
        defined(ARDUINO_ESP32S2_THING_PLUS) ||                          \
        defined(ARDUINO_FEATHERS2) ||                                   \
        defined(ARDUINO_FEATHERS2NEO) ||                                \
        defined(ARDUINO_TINYS2) ||                                      \
        defined(ARDUINO_ESP32S3_DEV) ||                                 \
        defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3) ||                    \
        defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_NOPSRAM) ||            \
        defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_TFT) ||                \
        defined(ARDUINO_ADAFRUIT_QTPY_ESP32S3_NOPSRAM) ||               \
        defined(ARDUINO_BeeMotionS3) ||                                 \
        defined(ARDUINO_Bee_S3) ||                                      \
        defined(ARDUINO_ESP32S3_CAM_LCD) ||                             \
        defined(ARDUINO_LOLIN_S3) ||                                    \
        defined(ARDUINO_TAMC_TERMOD_S3) ||                              \
        defined(ARDUINO_FEATHERS3) ||                                   \
        defined(ARDUINO_PROS3) ||                                       \
        defined(ARDUINO_TINYS3) ||                                      \
        defined(ARDUINO_WiFiduino32S3) ||                               \
        defined(ARDUINO_ESP32_S3_BOX)
        #define ERA_CHIP_TEMP
    #elif defined(ARDUINO_ARCH_ESP32)
        #define ERA_CHIP_TEMP_DEPRECATED
    #else
        #define ERA_CHIP_TEMP
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP32C3_DEV) ||                             \
            defined(ARDUINO_ADAFRUIT_QTPY_ESP32C3) ||                   \
            defined(ARDUINO_AirM2M_CORE_ESP32C3) ||                     \
            defined(ARDUINO_LOLIN_C3_MINI) ||                           \
            defined(ARDUINO_XIAO_ESP32C3) ||                            \
            defined(ARDUINO_WiFiduinoV2) ||                             \
            defined(ARDUINO_TTGO_T_OI_PLUS_DEV)
            #define ERA_BOARD_TYPE      "ESP32C3"
        #elif defined(ARDUINO_ESP32S2_DEV) ||                           \
            defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT) ||            \
            defined(ARDUINO_FUNHOUSE_ESP32S2) ||                        \
            defined(ARDUINO_MAGTAG29_ESP32S2) ||                        \
            defined(ARDUINO_METRO_ESP32S2) ||                           \
            defined(ARDUINO_ADAFRUIT_QTPY_ESP32S2) ||                   \
            defined(ARDUINO_atmegazero_esp32s2) ||                      \
            defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM) ||        \
            defined(ARDUINO_FRANZININHO_WIFI) ||                        \
            defined(ARDUINO_FRANZININHO_WIFI_MSC) ||                    \
            defined(ARDUINO_LOLIN_S2_MINI) ||                           \
            defined(ARDUINO_LOLIN_S2_PICO) ||                           \
            defined(ARDUINO_MICROS2) ||                                 \
            defined(ARDUINO_DEPARTMENT_OF_ALCHEMY_MINIMAIN_ESP32S2) ||  \
            defined(ARDUINO_ESP32S2_THING_PLUS) ||                      \
            defined(ARDUINO_FEATHERS2) ||                               \
            defined(ARDUINO_FEATHERS2NEO) ||                            \
            defined(ARDUINO_TINYS2)
            #define ERA_BOARD_TYPE      "ESP32S2"
        #elif defined(ARDUINO_ESP32S3_DEV) ||                           \
            defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3) ||                \
            defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_NOPSRAM) ||        \
            defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_TFT) ||            \
            defined(ARDUINO_ADAFRUIT_QTPY_ESP32S3_NOPSRAM) ||           \
            defined(ARDUINO_BeeMotionS3) ||                             \
            defined(ARDUINO_Bee_S3) ||                                  \
            defined(ARDUINO_ESP32S3_CAM_LCD) ||                         \
            defined(ARDUINO_LOLIN_S3) ||                                \
            defined(ARDUINO_TAMC_TERMOD_S3) ||                          \
            defined(ARDUINO_FEATHERS3) ||                               \
            defined(ARDUINO_PROS3) ||                                   \
            defined(ARDUINO_TINYS3) ||                                  \
            defined(ARDUINO_WiFiduino32S3)
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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP8266_NODEMCU)
            #define ERA_BOARD_TYPE      "NodeMCU"
        #elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
            #define ERA_BOARD_TYPE      "NodeMCUv2"
        #elif defined(ARDUINO_ARCH_ESP8266)
            #define ERA_BOARD_TYPE      "ESP8266"
        #else
            #define ERA_BOARD_TYPE      "ESP8266"
        #endif
    #endif

#elif defined(ARDUINO) &&                           \
    (defined(STM32F0xx) || defined(STM32F1xx) ||    \
    defined(STM32F2xx) || defined(STM32F3xx))

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         512
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

    #if defined(ERA_NO_RTOS)
        #define ERA_NO_YIELD
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(STM32F0xx)
            #define ERA_BOARD_TYPE      "STM32F0"
        #elif defined(STM32F1xx)
            #define ERA_BOARD_TYPE      "STM32F1"
        #elif defined(STM32F2xx)
            #define ERA_BOARD_TYPE      "STM32F2"
        #elif defined(STM32F3xx)
            #define ERA_BOARD_TYPE      "STM32F3"
        #endif
    #endif

#elif defined(ARDUINO) &&                           \
    (defined(STM32F4xx) || defined(STM32F7xx) ||    \
    defined(STM32H7xx))

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    5120
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #define ERA_100_PINS

    #if defined(ERA_NO_RTOS)
        #define ERA_NO_YIELD
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(STM32F4xx)
            #define ERA_BOARD_TYPE      "STM32F4"
        #elif defined(STM32F7xx)
            #define ERA_BOARD_TYPE      "STM32F7"
        #elif defined(STM32H7xx)
            #define ERA_BOARD_TYPE      "STM32H7"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_STM32)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         512
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        100
    #endif

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

    #if defined(ERA_NO_RTOS)
        #define ERA_NO_YIELD
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(STM32G0xx)
            #define ERA_BOARD_TYPE      "STM32G0"
        #elif defined(STM32G4xx)
            #define ERA_BOARD_TYPE      "STM32G4"
        #elif defined(STM32L0xx)
            #define ERA_BOARD_TYPE      "STM32L0"
        #elif defined(STM32L1xx)
            #define ERA_BOARD_TYPE      "STM32L1"
        #elif defined(STM32L4xx)
            #define ERA_BOARD_TYPE      "STM32L4"
        #elif defined(STM32L5xx)
            #define ERA_BOARD_TYPE      "STM32L5"
        #elif defined(STM32U5xx)
            #define ERA_BOARD_TYPE      "STM32U5"
        #elif defined(STM32WBxx)
            #define ERA_BOARD_TYPE      "STM32WB"
        #elif defined(STM32WLxx)
            #define ERA_BOARD_TYPE      "STM32WL"
        #else
            #define ERA_BOARD_TYPE      "STM32xx"
        #endif
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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     100
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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     100
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

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #define ERA_BOARD_TYPE          "Custom"
    #endif

#endif

#if !defined(ERA_MQTT_TX_BUFFER_SIZE)
    #define ERA_MQTT_TX_BUFFER_SIZE    2048
#endif

#endif /* INC_ERA_DETECT_HPP_ */
