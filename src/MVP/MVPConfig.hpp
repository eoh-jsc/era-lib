#ifndef INC_MVP_CONFIG_HPP_
#define INC_MVP_CONFIG_HPP_

#define MVP_FIRMWARE_VERSION        "1.0.0"

#define MAX_TOPIC_LENGTH            100
#define BASE_TOPIC_MQTT             "eoh/chip"

#ifdef MVP_AUTH_TOKEN
    #define MQTT_CLIENT_ID          MVP_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_CLIENT_ID)
    #define MQTT_CLIENT_ID          DEFAULT_MQTT_CLIENT_ID
#else
    #define MQTT_CLIENT_ID          ""
#endif

#ifdef DEFAULT_MQTT_HOST
    #define MVP_MQTT_HOST           DEFAULT_MQTT_HOST
#else
    #define MVP_MQTT_HOST           "remarkable-accountant.cloudmqtt.com"
#endif

#ifdef DEFAULT_MQTT_PORT
    #define MVP_MQTT_PORT           DEFAULT_MQTT_PORT
#else
    #define MVP_MQTT_PORT           1883
#endif

#ifdef MVP_AUTH_TOKEN
    #define MVP_MQTT_USERNAME       MVP_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_USERNAME)
    #define MVP_MQTT_USERNAME       DEFAULT_MQTT_USERNAME
#else
    #define MVP_MQTT_USERNAME       ""
#endif

#ifdef MVP_AUTH_TOKEN
    #define MVP_MQTT_PASSWORD       MVP_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_PASSWORD)
    #define MVP_MQTT_PASSWORD       DEFAULT_MQTT_PASSWORD
#else
    #define MVP_MQTT_PASSWORD       ""
#endif

#ifdef DEFAULT_BUFFER_SIZE
    #define MVP_BUFFER_SIZE         DEFAULT_BUFFER_SIZE
#else
    #define MVP_BUFFER_SIZE         1024
#endif

#define LIMIT_CONNECT_BROKER_MQTT   10

#endif