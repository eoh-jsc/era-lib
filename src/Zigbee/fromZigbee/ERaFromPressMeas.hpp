#ifndef INC_ERA_FROM_PRESSURE_MEASUREMENT_ZIGBEE_HPP_
#define INC_ERA_FROM_PRESSURE_MEASUREMENT_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::pressureMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclPressMeasSvrAttrT::ZCL_PRESS_MEAS_ATTR_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_PRESSURE, MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclPressMeasSvrAttrT::ZCL_PRESS_MEAS_ATTR_MIN_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_PRESSURE, MIN_MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclPressMeasSvrAttrT::ZCL_PRESS_MEAS_ATTR_MAX_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_PRESSURE, MAX_MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclPressMeasSvrAttrT::ZCL_PRESS_MEAS_ATTR_TOLERANCE:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_PRESSURE, TOLERANCE), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_PRESSURE_MEASUREMENT_ZIGBEE_HPP_ */
