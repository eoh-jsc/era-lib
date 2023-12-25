#ifndef INC_ERA_FROM_TEMPERATURE_MEASUREMENT_ZIGBEE_HPP_
#define INC_ERA_FROM_TEMPERATURE_MEASUREMENT_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::temperatureMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclTempMeasSvrAttrT::ZCL_TEMP_MEAS_ATTR_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_TEMPERATURE, MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclTempMeasSvrAttrT::ZCL_TEMP_MEAS_ATTR_MIN_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_TEMPERATURE, MIN_MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclTempMeasSvrAttrT::ZCL_TEMP_MEAS_ATTR_MAX_MEAS_VAL:
            this->addDataZigbee<int16_t>(afMsg, root, KEY(MEAS_TEMPERATURE, MAX_MEAS_VAL), static_cast<int16_t>(value));
            break;
        case ZbZclTempMeasSvrAttrT::ZCL_TEMP_MEAS_ATTR_TOLERANCE:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_TEMPERATURE, TOLERANCE), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_TEMPERATURE_MEASUREMENT_ZIGBEE_HPP_ */
