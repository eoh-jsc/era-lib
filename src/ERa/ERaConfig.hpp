#ifndef INC_ERA_CONFIG_HPP_
#define INC_ERA_CONFIG_HPP_

#if !defined(ERA_MODEL_TYPE)
    #define ERA_MODEL_TYPE              "ERa"
#endif
#define ERA_FIRMWARE_VERSION            "1.0.0"

#define MAX_TOPIC_LENGTH                100
#define BASE_TOPIC_MQTT                 "eoh/chip"

#if defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_CLIENT_ID          ERA_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_CLIENT_ID)
    #define ERA_MQTT_CLIENT_ID          DEFAULT_MQTT_CLIENT_ID
#else
    #define ERA_MQTT_CLIENT_ID          ""
#endif

#if defined(DEFAULT_MQTT_HOST)
    #define ERA_MQTT_HOST               DEFAULT_MQTT_HOST
#else
    #define ERA_MQTT_HOST               "remarkable-accountant.cloudmqtt.com"
#endif

#if defined(DEFAULT_MQTT_PORT)
    #define ERA_MQTT_PORT               DEFAULT_MQTT_PORT
#else
    #if defined(ERA_MQTT_SSL)
        #define ERA_MQTT_PORT           8883
    #else
        #define ERA_MQTT_PORT           1883
    #endif
#endif

#if defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_USERNAME           ERA_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_USERNAME)
    #define ERA_MQTT_USERNAME           DEFAULT_MQTT_USERNAME
#else
    #define ERA_MQTT_USERNAME           ""
#endif

#if defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_PASSWORD           ERA_AUTH_TOKEN
#elif defined(DEFAULT_MQTT_PASSWORD)
    #define ERA_MQTT_PASSWORD           DEFAULT_MQTT_PASSWORD
#else
    #define ERA_MQTT_PASSWORD           ""
#endif

#if defined(DEFAULT_MQTT_KEEP_ALIVE)
    #define ERA_MQTT_KEEP_ALIVE         DEFAULT_MQTT_KEEP_ALIVE
#else
    #define ERA_MQTT_KEEP_ALIVE         60
#endif

#if defined(DEFAULT_MQTT_PUBLISH_QOS)
    #define ERA_MQTT_PUBLISH_QOS        DEFAULT_MQTT_PUBLISH_QOS
#else
    #define ERA_MQTT_PUBLISH_QOS        0
#endif

#if defined(DEFAULT_MQTT_PUBLISH_RETAINED)
    #define ERA_MQTT_PUBLISH_RETAINED   DEFAULT_MQTT_PUBLISH_RETAINED
#else
    #define ERA_MQTT_PUBLISH_RETAINED   true
#endif

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE              "WiFi"
#endif

#define LIMIT_CONNECT_BROKER_MQTT       10

#define INFO_BOARD                      "board"
#define INFO_MODEL                      "model"
#define INFO_AUTH_TOKEN                 "auth_token"
#define INFO_FIRMWARE_VERSION           "firmware_version"
#define INFO_SSID                       "ssid"
#define INFO_BSSID                      "bssid"
#define INFO_RSSI                       "rssi"
#define INFO_MAC                        "mac"
#define INFO_LOCAL_IP                   "ip"
#define INFO_PING                       "ping"

#define INFO_MB_CHIP_TEMPERATURE        "chip_temperature"
#define INFO_MB_TEMPERATURE             "temperature"
#define INFO_MB_VOLTAGE                 "voltage"
#define INFO_MB_IS_BATTERY              "is_battery"
#define INFO_MB_RSSI                    "rssi"
#define INFO_MB_WIFI_USING              "wifi_is_using"

#include <ERa/ERaDetect.hpp>

#endif /* INC_ERA_CONFIG_HPP_ */
