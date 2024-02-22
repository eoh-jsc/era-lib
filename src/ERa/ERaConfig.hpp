#ifndef INC_ERA_CONFIG_HPP_
#define INC_ERA_CONFIG_HPP_

#if !defined(ERA_MODEL_TYPE)
    #if defined(BOARD_MODEL_NAME)
        #define ERA_MODEL_TYPE          BOARD_MODEL_NAME
    #else
        #define ERA_MODEL_TYPE          "ERa"
    #endif
#endif

#include <ERa/ERaVersion.hpp>

#define ERA_DEFAULT_PORT                80
#define ERA_DEFAULT_DOMAIN              "nossl-backend.eoh.io"
#define ERA_DEFAULT_PORT_SSL            443
#define ERA_DEFAULT_DOMAIN_SSL          "backend.eoh.io"

#define ERA_DEFAULT_MQTT_PORT           1883
#define ERA_DEFAULT_MQTT_PORT_SSL       8883

#if defined(ERA_AUTH_TOKEN)
    #define ERA_AUTHENTICATION_TOKEN    ERA_AUTH_TOKEN
#else
    #define ERA_AUTHENTICATION_TOKEN    ""
#endif

#define MAX_TOPIC_LENGTH                100

#if defined(DEFAULT_MQTT_BASE_TOPIC)
    #define ERA_MQTT_BASE_TOPIC         DEFAULT_MQTT_BASE_TOPIC
#elif defined(ERA_MQTT_BASE_TOPIC)
#else
    #define ERA_MQTT_BASE_TOPIC         "eoh/chip"
#endif

#if defined(DEFAULT_MQTT_CLIENT_ID)
    #define ERA_MQTT_CLIENT_ID          DEFAULT_MQTT_CLIENT_ID
#elif defined(ERA_BOARD_ID)
    #define ERA_MQTT_CLIENT_ID          ERA_BOARD_ID
#elif defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_CLIENT_ID          ERA_AUTH_TOKEN
#else
    #define ERA_MQTT_CLIENT_ID          ""
#endif

#if defined(DEFAULT_MQTT_HOST)
    #define ERA_MQTT_HOST               DEFAULT_MQTT_HOST
#else
    #if defined(ERA_LOCATION_VN)
        #define ERA_MQTT_HOST           "mqtt1.eoh.io"
    #elif defined(ERA_LOCATION_SG)
        #define ERA_MQTT_HOST           "remarkable-accountant.cloudmqtt.com"
    #else
        #pragma message "Default VN MQTT server"
        #define ERA_MQTT_HOST           "mqtt1.eoh.io"
    #endif
#endif

#if defined(DEFAULT_MQTT_PORT)
    #define ERA_MQTT_PORT               DEFAULT_MQTT_PORT
#else
    #if defined(ERA_MQTT_SSL)
        #define ERA_OTA_SSL
        #define ERA_MQTT_PORT           ERA_DEFAULT_MQTT_PORT_SSL
    #else
        #define ERA_MQTT_PORT           ERA_DEFAULT_MQTT_PORT
    #endif
#endif

#if defined(DEFAULT_MQTT_USERNAME)
    #define ERA_MQTT_USERNAME           DEFAULT_MQTT_USERNAME
#elif defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_USERNAME           ERA_AUTH_TOKEN
#else
    #define ERA_MQTT_USERNAME           ""
#endif

#if defined(DEFAULT_MQTT_PASSWORD)
    #define ERA_MQTT_PASSWORD           DEFAULT_MQTT_PASSWORD
#elif defined(ERA_AUTH_TOKEN)
    #define ERA_MQTT_PASSWORD           ERA_AUTH_TOKEN
#else
    #define ERA_MQTT_PASSWORD           ""
#endif

#if defined(DEFAULT_MQTT_KEEP_ALIVE)
    #define ERA_MQTT_KEEP_ALIVE         DEFAULT_MQTT_KEEP_ALIVE
#else
    #define ERA_MQTT_KEEP_ALIVE         60
#endif

#if defined(DEFAULT_MQTT_SUBSCRIBE_QOS)
    #define ERA_MQTT_SUBSCRIBE_QOS      DEFAULT_MQTT_SUBSCRIBE_QOS
#else
    #define ERA_MQTT_SUBSCRIBE_QOS      1
#endif

#if defined(DEFAULT_MQTT_PUBLISH_QOS)
    #define ERA_MQTT_PUBLISH_QOS        DEFAULT_MQTT_PUBLISH_QOS
#else
    #define ERA_MQTT_PUBLISH_QOS        1
#endif

#if defined(DEFAULT_MQTT_PUBLISH_RETAINED)
    #define ERA_MQTT_PUBLISH_RETAINED   DEFAULT_MQTT_PUBLISH_RETAINED
#else
    #define ERA_MQTT_PUBLISH_RETAINED   true
#endif

#if defined(DEFAULT_SOCKET_TIMEOUT)
    #define ERA_SOCKET_TIMEOUT          DEFAULT_SOCKET_TIMEOUT
#else
    #define ERA_SOCKET_TIMEOUT          1
#endif

#if !defined(ERA_DISABLE_SYNC_CONFIG)
    #define ERA_ASK_CONFIG_WHEN_RESTART
#endif

#if defined(ERA_VENDOR_NAME)
    #define ERA_ORG_NAME                ERA_VENDOR_NAME
#elif !defined(ERA_ORG_NAME)
    #define ERA_ORG_NAME                "eoh"
#endif

#if defined(ERA_VENDOR_PREFIX)
    #define ERA_MODEL_NAME              ERA_VENDOR_PREFIX
#elif defined(ERA_PREFIX_NAME)
    #define ERA_MODEL_NAME              ERA_PREFIX_NAME
#elif !defined(ERA_MODEL_NAME)
    #define ERA_MODEL_NAME              "era"
#endif

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE            "WiFi"
#endif

#if defined(ERA_LIMIT_CONNECT)
    #define LIMIT_CONNECT_BROKER_MQTT   ERA_LIMIT_CONNECT
#else
    #define LIMIT_CONNECT_BROKER_MQTT   5
#endif

#define INFO_BOARD                      "board"
#define INFO_MODEL                      "model"
#define INFO_BOARD_ID                   "board_id"
#define INFO_IMEI                       "imei"
#define INFO_AUTH_TOKEN                 "auth_token"
#define INFO_BUILD_DATE                 "build"
#define INFO_VERSION                    "version"
#define INFO_MCU_VERSION                "mcu_version"
#define INFO_FIRMWARE_VERSION           "firmware_version"
#define INFO_PLUG_AND_PLAY              "plug_and_play"
#define INFO_SIGNAL_STRENGTH            "signal_strength"
#define INFO_NETWORK_PROTOCOL           "network_protocol"
#define INFO_SSID                       "ssid"
#define INFO_PASS                       "pass"
#define INFO_BSSID                      "bssid"
#define INFO_RSSI                       "rssi"
#define INFO_MAC                        "mac"
#define INFO_LOCAL_IP                   "ip"
#define INFO_SSL                        "ssl"
#define INFO_PING                       "ping"
#define INFO_FREE_RAM                   "free_ram"
#define INFO_UPTIME                     "uptime"
#define INFO_RESET_REASON               "reset_reason"
#define BUILD_DATE_TIME                 __DATE__ " " __TIME__

#define INFO_MB_DATA                    "data"
#define INFO_MB_ACK                     "ack"
#define INFO_MB_SCAN                    "scan"
#define INFO_MB_READ_FAIL               "modbus_fail"
#define INFO_MB_WRITE_FAIL              "modbus_write_fail"
#define INFO_MB_TOTAL                   "modbus_total"
#define INFO_MB_CHIP_TEMPERATURE        "chip_temperature"
#define INFO_MB_TEMPERATURE             "temperature"
#define INFO_MB_VOLTAGE                 "voltage"
#define INFO_MB_IS_BATTERY              "is_battery"
#define INFO_MB_RSSI                    "rssi"
#define INFO_MB_SIGNAL_STRENGTH         "signal_strength"
#define INFO_MB_WIFI_USING              "wifi_is_using"

#include <ERa/ERaDetect.hpp>
#include <ERa/ERaTopic.hpp>

#endif /* INC_ERA_CONFIG_HPP_ */
