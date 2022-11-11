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

#endif

#endif /* INC_ERA_DETECT_HPP_ */
