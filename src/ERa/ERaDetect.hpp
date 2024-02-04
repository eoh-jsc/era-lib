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

#if !defined(ERA_STRING_CAPACITY_MAX)
    #if defined(BOARD_HAS_PSRAM)
        #define ERA_STRING_CAPACITY_MAX 3145728
    #else
        #define ERA_STRING_CAPACITY_MAX 65535
    #endif
#endif

    #define ERA_BT
    #define ERA_OTA

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP32C2_DEV) ||                             \
            defined(CONFIG_IDF_TARGET_ESP32C2)
            #if !defined(ARDUINO_ESP32C2_DEV)
                #define ARDUINO_ESP32C2_DEV
            #endif
            #define ERA_BOARD_TYPE      "ESP32C2"
        #elif defined(ARDUINO_ESP32C3_DEV) ||                           \
            defined(ARDUINO_ADAFRUIT_QTPY_ESP32C3) ||                   \
            defined(ARDUINO_AirM2M_CORE_ESP32C3) ||                     \
            defined(ARDUINO_LOLIN_C3_MINI) ||                           \
            defined(ARDUINO_XIAO_ESP32C3) ||                            \
            defined(ARDUINO_WiFiduinoV2) ||                             \
            defined(ARDUINO_TTGO_T_OI_PLUS_DEV) ||                      \
            defined(CONFIG_IDF_TARGET_ESP32C3)
            #if !defined(ARDUINO_ESP32C3_DEV)
                #define ARDUINO_ESP32C3_DEV
            #endif
            #define ERA_BOARD_TYPE      "ESP32C3"
        #elif defined(ARDUINO_ESP32C6_DEV) ||                           \
            defined(CONFIG_IDF_TARGET_ESP32C6)
            #define ERA_BOARD_TYPE      "ESP32C6"
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
            defined(ARDUINO_TINYS2) ||                                  \
            defined(CONFIG_IDF_TARGET_ESP32S2)
            #if !defined(ARDUINO_ESP32S2_DEV)
                #define ARDUINO_ESP32S2_DEV
            #endif
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
            defined(ARDUINO_WiFiduino32S3) ||                           \
            defined(CONFIG_IDF_TARGET_ESP32S3)
            #if !defined(ARDUINO_ESP32S3_DEV)
                #define ARDUINO_ESP32S3_DEV
            #endif
            #define ERA_BOARD_TYPE      "ESP32S3"
        #elif defined(ARDUINO_ESP32_S3_BOX)
            #define ERA_BOARD_TYPE      "ESP32S3-BOX"
        #elif defined(ARDUINO_ESP32H2_DEV) ||                           \
            defined(CONFIG_IDF_TARGET_ESP32H2)
            #if !defined(ARDUINO_ESP32H2_DEV)
                #define ARDUINO_ESP32H2_DEV
            #endif
            #define ERA_BOARD_TYPE      "ESP32H2"
        #elif defined(ARDUINO_ESP32_PICO)
            #define ERA_BOARD_TYPE      "ESP32-PICO"
        #elif defined(ARDUINO_ARCH_ESP32) ||                            \
            defined(CONFIG_IDF_TARGET_ESP32)
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

    #define ERA_OTA

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_ESP8266_NODEMCU)
            #define ERA_BOARD_TYPE      "NodeMCU"
        #elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
            #define ERA_BOARD_TYPE      "NodeMCUv2"
        #elif defined(ARDUINO_ESP8266_ESP01)
            #define ERA_BOARD_TYPE      "ESP01"
        #elif defined(ARDUINO_ESP8266_ESP07)
            #define ERA_BOARD_TYPE      "ESP07"
        #elif defined(ARDUINO_ESP8266_ESP12)
            #define ERA_BOARD_TYPE      "ESP12"
        #elif defined(ARDUINO_ESP8266_ESP13)
            #define ERA_BOARD_TYPE      "ESP13"
        #elif defined(ARDUINO_ESP8266_ESP210)
            #define ERA_BOARD_TYPE      "ESP210"
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

    #define ERA_OTA

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

    #if !defined(ERA_WATCHDOG_TIMEOUT)
        #define ERA_WATCHDOG_TIMEOUT    32760
    #endif

    #if defined(STM32F4xx) ||   \
        defined(STM32F7xx)
        #define ERA_OTA
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

    #if !defined(ERA_WATCHDOG_TIMEOUT)
        #define ERA_WATCHDOG_TIMEOUT    32760
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

    #if !defined(__MBED__)
        #define ERA_OTA
    #endif

    #if !defined(ERA_WATCHDOG_TIMEOUT)
        #define ERA_WATCHDOG_TIMEOUT    8389
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_NANO_RP2040_CONNECT)
            #define ERA_BOARD_TYPE      "Arduino NANO RP2040"
        #elif defined(ARDUINO_RASPBERRY_PI_PICO)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico"
        #elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico W"
        #elif defined(ARDUINO_ARCH_RP2040)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico"
        #else
            #define ERA_BOARD_TYPE      "RP2040"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_NRF5)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    2048
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         1024
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        200
    #endif

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #if defined(NRF52_SERIES)
            #define ERA_MAX_VIRTUAL_PIN 100
        #else
            #define ERA_MAX_VIRTUAL_PIN 50
        #endif
    #endif

    #if defined(NRF52_SERIES)
        #define ERA_100_PINS
    #endif

    #define ERA_IGNORE_STD_FUNCTIONAL_STRING

    #if !defined(ERA_BOARD_TYPE)
        #if defined(NRF52_SERIES)
            #define ERA_BOARD_TYPE      "nRF52"
        #else
            #define ERA_BOARD_TYPE      "nRF51"
        #endif
    #endif

#elif defined(ARDUINO) && (defined(ARDUINO_ARCH_SAM) || \
    defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ARC32))

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
        #if defined(ARDUINO_ARCH_SAM)
            #define ERA_MAX_VIRTUAL_PIN 100
        #else
            #define ERA_MAX_VIRTUAL_PIN 50
        #endif
    #endif

    #if defined(ARDUINO_ARCH_SAMD)
        #define ERA_USE_ERA_DTOSTRF
    #endif

    #if defined(ARDUINO_ARCH_SAM)
        #define ERA_100_PINS
        #define ERA_UNUSED_STD_NOTHROW
    #endif

    #if !defined(ARDUINO_ARCH_SAMD)
        #define ERA_IGNORE_INPUT_PULLDOWN
    #endif

    #if defined(ARDUINO_ARCH_SAMD)
        #if !defined(ERA_WATCHDOG_TIMEOUT)
            #define ERA_WATCHDOG_TIMEOUT    16000
        #endif
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_SAMD_ZERO)
            #define ERA_BOARD_TYPE      "Arduino Zero"
        #elif defined(ARDUINO_SAMD_MKR1000)
            #define ERA_BOARD_TYPE      "MKR1000"
        #elif defined(ARDUINO_SAMD_MKRZERO)
            #define ERA_BOARD_TYPE      "MKRZERO"
        #elif defined(ARDUINO_SAMD_MKRNB1500)
            #define ERA_BOARD_TYPE      "MKR NB 1500"
        #elif defined(ARDUINO_SAMD_MKRGSM1400)
            #define ERA_BOARD_TYPE      "MKR GSM 1400"
        #elif defined(ARDUINO_SAMD_MKRWAN1300)
            #define ERA_BOARD_TYPE      "MKR WAN 1300"
        #elif defined(ARDUINO_SAMD_MKRFox1200)
            #define ERA_BOARD_TYPE      "MKR FOX 1200"
        #elif defined(ARDUINO_SAMD_MKRWIFI1010)
            #define ERA_BOARD_TYPE      "MKR WiFi 1010"
        #elif defined(ARDUINO_SAMD_MKRVIDOR4000)
            #define ERA_BOARD_TYPE      "MKR Vidor 4000"
        #elif defined(ARDUINO_SAM_DUE)
            #define ERA_BOARD_TYPE      "Arduino Due"
        #elif defined(ARDUINO_SAMD_NANO_33_IOT)
            #define ERA_BOARD_TYPE      "Arduino NANO 33 IoT"
        #elif defined(TARGET_ARDUINO_NANO33BLE) ||  \
            defined(ARDUINO_ARDUINO_NANO33BLE) ||   \
            defined(ARDUINO_NANO33BLE)
            #define ERA_BOARD_TYPE      "Arduino NANO 33 BLE"
        #elif defined(ARDUINO_ARCH_ARC32)
            #define ERA_BOARD_TYPE      "Arduino 101"
        #else
            #define ERA_BOARD_TYPE      "Arduino SAMD"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_AVR)

    #if !defined(ERA_MQTT_BUFFER_SIZE)
        #define ERA_MQTT_BUFFER_SIZE    512
    #endif

    #if !defined(ERA_BUFFER_SIZE)
        #define ERA_BUFFER_SIZE         512
    #endif

    #if !defined(ERA_MAX_GPIO_PIN)
        #define ERA_MAX_GPIO_PIN        50
    #endif

    #if !defined(ERA_MAX_VIRTUAL_PIN)
        #define ERA_MAX_VIRTUAL_PIN     50
    #endif

    #define ERA_USE_ERA_ATOLL
    #define ERA_IGNORE_INPUT_PULLDOWN
    #define ERA_VIRTUAL_WRITE_LEGACY

    #if !defined(ERA_WATCHDOG_TIMEOUT)
        #define ERA_WATCHDOG_TIMEOUT    8000
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_AVR_NANO)
            #define ERA_BOARD_TYPE      "Arduino Nano"
        #elif defined(ARDUINO_AVR_UNO) || \
            defined(ARDUINO_AVR_DUEMILANOVE)
            #define ERA_BOARD_TYPE      "Arduino Uno"
        #elif defined(ARDUINO_AVR_YUN)
            #define ERA_BOARD_TYPE      "Arduino Yun"
        #elif defined(ARDUINO_AVR_MINI)
            #define ERA_BOARD_TYPE      "Arduino Mini"
        #elif defined(ARDUINO_AVR_ETHERNET)
            #define ERA_BOARD_TYPE      "Arduino Ethernet"
        #elif defined(ARDUINO_AVR_FIO)
            #define ERA_BOARD_TYPE      "Arduino Fio"
        #elif defined(ARDUINO_AVR_BT)
            #define ERA_BOARD_TYPE      "Arduino BT"
        #elif defined(ARDUINO_AVR_PRO)
            #define ERA_BOARD_TYPE      "Arduino Pro"
        #elif defined(ARDUINO_AVR_NG)
            #define ERA_BOARD_TYPE      "Arduino NG"
        #elif defined(ARDUINO_AVR_GEMMA)
            #define ERA_BOARD_TYPE      "Arduino Gemma"
        #elif defined(ARDUINO_AVR_MEGA) || \
            defined(ARDUINO_AVR_MEGA2560)
            #define ERA_BOARD_TYPE      "Arduino Mega"
        #elif defined(ARDUINO_AVR_ADK)
            #define ERA_BOARD_TYPE      "Arduino Mega ADK"
        #elif defined(ARDUINO_AVR_LEONARDO)
            #define ERA_BOARD_TYPE      "Arduino Leonardo"
        #elif defined(ARDUINO_AVR_MICRO)
            #define ERA_BOARD_TYPE      "Arduino Micro"
        #elif defined(ARDUINO_AVR_ESPLORA)
            #define ERA_BOARD_TYPE      "Arduino Esplora"
        #elif defined(ARDUINO_AVR_LILYPAD)
            #define ERA_BOARD_TYPE      "Lilypad"
        #elif defined(ARDUINO_AVR_LILYPAD_USB)
            #define ERA_BOARD_TYPE      "Lilypad USB"
        #elif defined(ARDUINO_AVR_ROBOT_MOTOR)
            #define ERA_BOARD_TYPE      "Robot Motor"
        #elif defined(ARDUINO_AVR_ROBOT_CONTROL)
            #define ERA_BOARD_TYPE      "Robot Control"
        #elif defined(ARDUINO_AVR_UNO_WIFI_REV2)
            #define ERA_BOARD_TYPE      "Arduino UNO WiFi Rev2"
        #else
            #define ERA_BOARD_TYPE      "Arduino AVR"
        #endif
    #endif

#elif defined(ARDUINO) &&                       \
    (defined(RTL8722DM) || defined(ARDUINO_AMEBA))

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
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #define ERA_100_PINS

    #define ERA_BT
    #define ERA_OTA

    #define ERA_IGNORE_INPUT_PULLDOWN
    #define ERA_IGNORE_STD_FUNCTIONAL_STRING

    #if !defined(ERA_BOARD_TYPE)
        #if defined(BOARD_RTL8722DM)
            #define ERA_BOARD_TYPE      "RTL8722DM_CSM"
        #elif defined(BOARD_RTL8722DM_MINI)
            #define ERA_BOARD_TYPE      "RTL8722DM_MINI"
        #elif defined(BOARD_RTL8720DN_BW16)
            #define ERA_BOARD_TYPE      "RTL8720DN"
        #elif defined(BOARD_RTL8721DM)
            #define ERA_BOARD_TYPE      "RTL8721DM"
        #elif defined(BOARD_RTL8720DF)
            #define ERA_BOARD_TYPE      "RTL8720DF"
        #elif defined(BOARD_RTL8710)
            #define ERA_BOARD_TYPE      "RTL8710"
        #elif defined(BOARD_RTL8711AM)
            #define ERA_BOARD_TYPE      "RTL8711AM"
        #elif defined(BOARD_RTL8195A)
            #define ERA_BOARD_TYPE      "RTL8195A"
        #else
            #define ERA_BOARD_TYPE      "RTL Ameba"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_RENESAS)

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
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #if defined(ARDUINO_PORTENTA_C33)
        #define ERA_USE_ERA_DTOSTRF
    #endif

    #define ERA_100_PINS

    #if !defined(ERA_WATCHDOG_TIMEOUT)
        #define ERA_WATCHDOG_TIMEOUT    32760
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_MINIMA)
            #define ERA_BOARD_TYPE      "Renasas Minima"
        #elif defined(ARDUINO_UNOWIFIR4)
            #define ERA_BOARD_TYPE      "Renasas UnoWiFiR4"
        #elif defined(ARDUINO_MUXTO)
            #define ERA_BOARD_TYPE      "Renasas MuxTo"
        #elif defined(ARDUINO_PORTENTA_C33)
            #define ERA_BOARD_TYPE      "Renasas Portenta C33"
        #else
            #define ERA_BOARD_TYPE      "Renasas"
        #endif
    #endif

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_ARM)

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
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #define ERA_100_PINS
    #define ERA_UNUSED_STD_NOTHROW

    #define ERA_OTA

    #define ERA_IGNORE_STD_FUNCTIONAL_STRING

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_QUECTEL_EC200U)
            #define ERA_BOARD_TYPE      "Quectel EC200U"
        #elif defined(ARDUINO_QUECTEL_EC600U)
            #define ERA_BOARD_TYPE      "Quectel EC600U"
        #elif defined(ARDUINO_EG915U)
            #define ERA_BOARD_TYPE      "Quectel EG915"
        #elif defined(ARDUINO_QUECTEL_M66TEA)
            #define ERA_BOARD_TYPE      "Quectel M66"
        #elif defined(ARDUINO_QUECTEL_MC20TEA)
            #define ERA_BOARD_TYPE      "Quectel MC20"
        #elif defined(ARDUINO_QUECTEL_MC60TEA)
            #define ERA_BOARD_TYPE      "Quectel MC60"
        #elif defined(ARDUINO_SIWI_S20GSM)
            #define ERA_BOARD_TYPE      "Quectel MC20UCB"
        #elif defined(ARDUINO_QUECTEL_M56TEA)
            #define ERA_BOARD_TYPE      "Quectel M56"
        #elif defined(ARDUINO_EC100N)
            #define ERA_BOARD_TYPE      "Quectel EC100N"
        #elif defined(ARDUINO_EC100Y)
            #define ERA_BOARD_TYPE      "Quectel EC100Y"
        #elif defined(ARDUINO_EC200N)
            #define ERA_BOARD_TYPE      "Quectel EC200N"
        #elif defined(ARDUINO_EC200S)
            #define ERA_BOARD_TYPE      "Quectel EC200S"
        #elif defined(ARDUINO_EC600N)
            #define ERA_BOARD_TYPE      "Quectel EC600N"
        #elif defined(ARDUINO_EC600S)
            #define ERA_BOARD_TYPE      "Quectel EC600S"
        #elif defined(ARDUINO_EG912Y)
            #define ERA_BOARD_TYPE      "Quectel EG912Y"
        #elif defined(ARDUINO_EG915N)
            #define ERA_BOARD_TYPE      "Quectel EG915N"
        #elif defined(ARDUINO_NEOWAY_N716)
            #define ERA_BOARD_TYPE      "Neoway N716"
        #elif defined(ARDUINO_NEOWAY_N58)
            #define ERA_BOARD_TYPE      "Neoway N58"
        #elif defined(ARDUINO_SIMCOM_SIM868)
            #define ERA_BOARD_TYPE      "Simcom SIM868"
        #elif defined(ARDUINO_A7670C)
            #define ERA_BOARD_TYPE      "Simcom SIMA7670C"
        #else
            #define ERA_BOARD_TYPE      "Logicrom"
        #endif
    #endif

#elif defined(ARDUINO) && defined(__MBED__)

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
        #if defined(ARDUINO_PORTENTA_H7_M4) ||  \
            defined(ARDUINO_PORTENTA_H7_M7) ||  \
            defined(ARDUINO_NICLA_VISION) ||    \
            defined(ARDUINO_OPTA) ||            \
            defined(ARDUINO_GIGA) ||            \
            defined(ARDUINO_ARCH_RP2040)
            #define ERA_MAX_VIRTUAL_PIN 100
        #else
            #define ERA_MAX_VIRTUAL_PIN 50
        #endif
    #endif

    #if defined(ARDUINO_ARCH_SAMD)
        #define ERA_USE_ERA_DTOSTRF
    #endif

    #if defined(ARDUINO_PORTENTA_H7_M4) ||      \
        defined(ARDUINO_PORTENTA_H7_M7) ||      \
        defined(ARDUINO_NICLA_VISION) ||        \
        defined(ARDUINO_OPTA) ||                \
        defined(ARDUINO_GIGA) ||                \
        defined(ARDUINO_ARCH_RP2040)
        #define ERA_100_PINS
    #endif

    #if defined(ARDUINO_NICLA_VISION) ||        \
        defined(ARDUINO_OPTA)
        /* Arduino Nicla and OPTA not support analogWrite */
        #define ERA_IGNORE_ANALOG_WRITE
    #endif

    #if defined(ARDUINO_PORTENTA_H7_M4) ||      \
        defined(ARDUINO_PORTENTA_H7_M7) ||      \
        defined(ARDUINO_NICLA_VISION) ||        \
        defined(ARDUINO_OPTA) ||                \
        defined(ARDUINO_GIGA)
        #define ARDUINO_MBED_HAS_WIFI
        #if !defined(ERA_WATCHDOG_TIMEOUT)
            #define ERA_WATCHDOG_TIMEOUT    32760
        #endif
    #elif defined(ARDUINO_NANO_RP2040_CONNECT) ||   \
        defined(ARDUINO_RASPBERRY_PI_PICO) ||       \
        defined(ARDUINO_RASPBERRY_PI_PICO_W) ||     \
        defined(ARDUINO_ARCH_RP2040)
        #if !defined(ERA_WATCHDOG_TIMEOUT)
            #define ERA_WATCHDOG_TIMEOUT    8389
        #endif
    #endif

    #if !defined(ERA_BOARD_TYPE)
        #if defined(ARDUINO_SAMD_NANO_33_IOT)
            #define ERA_BOARD_TYPE      "Arduino NANO 33 IoT"
        #elif defined(ARDUINO_NANO_RP2040_CONNECT)
            #define ERA_BOARD_TYPE      "Arduino NANO RP2040"
        #elif defined(ARDUINO_RASPBERRY_PI_PICO)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico"
        #elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico W"
        #elif defined(ARDUINO_ARCH_RP2040)
            #define ERA_BOARD_TYPE      "Raspberry Pi Pico"
        #elif defined(ARDUINO_PORTENTA_H7_M4) ||    \
            defined(ARDUINO_PORTENTA_H7_M7)
            #define ERA_BOARD_TYPE      "Arduino Portenta H7"
        #elif defined(ARDUINO_NICLA_VISION)
            #define ERA_BOARD_TYPE      "Arduino Nicla Vision"
        #elif defined(ARDUINO_OPTA)
            #define ERA_BOARD_TYPE      "Arduino OPTA"
        #else
            #define ERA_BOARD_TYPE      "Arduino Mbed"
        #endif
    #endif

#elif defined(PARTICLE) || defined(SPARK)

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
        #define ERA_MAX_VIRTUAL_PIN     100
    #endif

    #define ERA_100_PINS

    #if !defined(ERA_BOARD_TYPE)
        #if PLATFORM_ID == 0
            #define ERA_BOARD_TYPE      "Particle Core"
        #elif PLATFORM_ID == 6
            #define ERA_BOARD_TYPE      "Particle Photon"
        #elif PLATFORM_ID == 8
            #define ERA_BOARD_TYPE      "Particle P1"
        #elif PLATFORM_ID == 9
            #define ERA_BOARD_TYPE      "Particle Ethernet"
        #elif PLATFORM_ID == 10
            #define ERA_BOARD_TYPE      "Particle Electron"
        #elif PLATFORM_ID == 12
            #define ERA_BOARD_TYPE      "Particle Argon"
        #elif PLATFORM_ID == 13
            #define ERA_BOARD_TYPE      "Particle Boron"
        #elif PLATFORM_ID == 14
            #define ERA_BOARD_TYPE      "Particle Xenon"
        #elif PLATFORM_ID == 26
            #define ERA_BOARD_TYPE      "Particle Tracker"
        #elif PLATFORM_ID == 31
            #define ERA_BOARD_TYPE      "Particle RPi"
        #elif PLATFORM_ID == 82
            #define ERA_BOARD_TYPE      "Digistump Oak"
        #elif PLATFORM_ID == 88
            #define ERA_BOARD_TYPE      "RedBear Duo"
        #elif PLATFORM_ID == 103
            #define ERA_BOARD_TYPE      "Bluz"
        #else
            #define ERA_BOARD_TYPE      "Particle"
        #endif
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

#if !defined(ERA_STRING_CAPACITY_MAX)
    #define ERA_STRING_CAPACITY_MAX     3145728
#endif

    #define ERA_OTA

    #if !defined(ERA_BOARD_TYPE)
        #if defined(RASPBERRY)
            #define ERA_BOARD_TYPE      "Raspberry"
        #elif defined(TINKER_BOARD)
            #define ERA_BOARD_TYPE      "Tinker Board"
        #elif defined(ORANGE_PI)
            #define ERA_BOARD_TYPE      "Orange Pi"
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
        #define ERA_BOARD_TYPE          "Unknown"
    #endif

#endif

#if !defined(ERA_MAX_PROPERTY)
    #define ERA_MAX_PROPERTY            ERA_MAX_VIRTUAL_PIN
#endif

#if !defined(ERA_MAX_REPORT)
    #define ERA_MAX_REPORT              ERA_MAX_VIRTUAL_PIN
#endif

#if !defined(ERA_MQTT_RX_BUFFER_SIZE)
    #define ERA_MQTT_RX_BUFFER_SIZE     ERA_MQTT_BUFFER_SIZE
#endif

#if !defined(ERA_MQTT_TX_BUFFER_SIZE)
    #define ERA_MQTT_TX_BUFFER_SIZE     256
#endif

#if !defined(ERA_MAX_READ_BYTES)
    #define ERA_MAX_READ_BYTES          512
#endif

#if !defined(ERA_MAX_WRITE_BYTES)
    #define ERA_MAX_WRITE_BYTES         512
#endif

#if !defined(ERA_OTA_BUFFER_SIZE)
    #define ERA_OTA_BUFFER_SIZE         256
#endif

#if !defined(ERA_WATCHDOG_TIMEOUT)
    #define ERA_WATCHDOG_TIMEOUT        60000UL
#endif

#if !defined(ERA_STRING_CAPACITY_MAX)
    #define ERA_STRING_CAPACITY_MAX     16384UL
#endif

#if !defined(ERA_GET_SIGNAL_TIMEOUT)
    #define ERA_GET_SIGNAL_TIMEOUT      60000UL
#endif

#define ERA_FATALITY_TIMEOUT            10000UL
#define ERA_HEARTBEAT_INTERVAL          3600000UL

#define ERA_STRING_REPORT_ON_WRITE

#include <limits.h>

#if defined(ERA_USE_LONG_LONG)
    typedef long long                   ERaInt_t;
    typedef unsigned long long          ERaUInt_t;

    #define ERA_INT_MAX                 LONG_LONG_MAX
    #define ERA_INT_MIN                 LONG_LONG_MIN
    #define ERA_INT_FORMAT              "%lli"
#else
    typedef long                        ERaInt_t;
    typedef unsigned long               ERaUInt_t;

    #define ERA_INT_MAX                 LONG_MAX
    #define ERA_INT_MIN                 LONG_MIN
    #define ERA_INT_FORMAT              "%li"
#endif

#if !defined(ERA_INTEGER_C_TYPE)
    #if defined(ERA_INT_FORMAT)
        #define ERA_INTEGER_C_TYPE      ERA_INT_FORMAT
    #else
        #define ERA_INTEGER_C_TYPE      "%d"
    #endif
#endif

#if defined(ERA_IGNORE_STD_FUNCTIONAL_STRING)
    /* OK, ignore the std functional */
#elif defined(__has_include)
    #if __has_include(<functional>)
        #include <functional>
        #define ERA_HAS_FUNCTIONAL_H
    #endif
#endif

#endif /* INC_ERA_DETECT_HPP_ */
