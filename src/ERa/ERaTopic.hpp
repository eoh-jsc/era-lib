#ifndef INC_ERA_TOPIC_HPP_
#define INC_ERA_TOPIC_HPP_

/*
Topic format:
    eoh/chip/<token>/<prefix topic>
*/

#define ERA_BASE_TOPIC                              "eoh/chip/%s"

/* LWT topic: Used to indicate whether it is currently online or not */
#define ERA_PREFIX_LWT_TOPIC                        "/is_online"

/* Subscribe topic */
#define ERA_SUB_PREFIX_DOWN_TOPIC                   "/down"
#define ERA_SUB_PREFIX_ARDUINO_TOPIC                "/arduino_pin/+"
#define ERA_SUB_PREFIX_VIRTUAL_TOPIC                "/virtual_pin/+"
#define ERA_SUB_PREFIX_ASK_WIFI_TOPIC               "/wifi/ask"
#define ERA_SUB_PREFIX_CHANGE_WIFI_TOPIC            "/wifi/change"
#define ERA_SUB_PREFIX_AUTOMATION_TOPIC             "/local_control"

#if defined(ERA_ZIGBEE) ||  \
    defined(ERA_SPECIFIC)
    #define ERA_SUB_PREFIX_ZIGBEE_GET_TOPIC         "/zigbee/+/get"
    #define ERA_SUB_PREFIX_ZIGBEE_DOWN_TOPIC        "/zigbee/+/down"
    #define ERA_SUB_PREFIX_ZIGBEE_JOIN_TOPIC        "/zigbee/permit_to_join"
    #define ERA_SUB_PREFIX_ZIGBEE_REMOVE_TOPIC      "/zigbee/remove_device"
#endif

/* Publish topic */
#define ERA_PUB_PREFIX_INFO_TOPIC                   "/info"
#define ERA_PUB_PREFIX_MODBUS_DATA_TOPIC            "/data"
#define ERA_PUB_PREFIX_CONFIG_DATA_TOPIC            "/config/" ERA_INTEGER_C_TYPE "/value"
#define ERA_PUB_PREFIX_MULTI_CONFIG_DATA_TOPIC      "/config_value"
#define ERA_PUB_PREFIX_SELF_SENSOR_TOPIC            "/self_sensor"
#define ERA_PUB_PREFIX_LIST_WIFI_TOPIC              "/wifi/list"
#define ERA_PUB_PREFIX_CHANGE_RESULT_WIFI_TOPIC     "/wifi/change_result"
#define ERA_PUB_PREFIX_NOTIFY_DATA_TOPIC            "/automate/" ERA_INTEGER_C_TYPE \
                                                    "/send_notify/" ERA_INTEGER_C_TYPE
#define ERA_PUB_PREFIX_EMAIL_DATA_TOPIC             "/automate/" ERA_INTEGER_C_TYPE \
                                                    "/send_email/" ERA_INTEGER_C_TYPE

/* For debug */
#if !defined(ERA_DEBUG_PREFIX)
    #define ERA_DEBUG_PREFIX                        "/debug"
#endif

/* Subscribe topic */
#define ERA_SUB_PREFIX_OP_DOWN_TOPIC                ERA_DEBUG_PREFIX "/+/down"

/* Publish topic */
#define ERA_PUB_PREFIX_PIN_DATA_TOPIC               ERA_DEBUG_PREFIX "/pin/data"
#define ERA_PUB_PREFIX_PIN_DOWN_TOPIC               ERA_DEBUG_PREFIX "/pin/down"

#define ERA_PUB_PREFIX_DOWN_TOPIC                   "/down"
#define ERA_PUB_PREFIX_ARDUINO_DOWN_TOPIC           "/arduino_pin/" ERA_INTEGER_C_TYPE
#define ERA_PUB_PREFIX_VIRTUAL_DOWN_TOPIC           "/virtual_pin/" ERA_INTEGER_C_TYPE

#define ERA_PUB_PREFIX_ZIGBEE_DOWN_TOPIC            "/zigbee/%s/down"

#endif /* INC_ERA_TOPIC_HPP_ */
