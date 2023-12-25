#ifndef INC_ERA_FROM_SCALE_ZIGBEE_HPP_
#define INC_ERA_FROM_SCALE_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

static const ScaleDataZigbee_t ScaleZigbeeList[] = {
    {
        KEY(MEAS_TEMPERATURE, MEAS_VAL), 0.01f, 100
    },
    {
        KEY(MEAS_HUMIDITY, MEAS_VAL), 0.01f, 100
    }
};

#endif /* INC_ERA_FROM_SCALE_ZIGBEE_HPP_ */
