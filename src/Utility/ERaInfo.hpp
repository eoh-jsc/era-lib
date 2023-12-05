#ifndef INC_ERA_INFO_HPP_
#define INC_ERA_INFO_HPP_

#include <stdint.h>
#include <ERa/ERaConfig.hpp>

static inline
bool ERaInfoSSL(uint16_t port = ERA_MQTT_PORT) {
    bool ssl {false};

    switch (port) {
        case ERA_DEFAULT_MQTT_PORT:
            ssl = false;
            break;
        case ERA_DEFAULT_MQTT_PORT_SSL:
            ssl = true;
            break;
        default:
#if defined(ERA_MQTT_SSL)
            ssl = true;
#else
            ssl = false;
#endif
            break;
    }

    return ssl;
}

#endif /* INC_ERA_INFO_HPP_ */
