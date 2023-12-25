#ifndef INC_ERA_FROM_HUMIDITY_MEASUREMENT_ZIGBEE_HPP_
#define INC_ERA_FROM_HUMIDITY_MEASUREMENT_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::humidityMeasFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclHumiMeasSvrAttrT::ZCL_HUMI_MEAS_ATTR_MEAS_VAL:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_HUMIDITY, MEAS_VAL), value);
            break;
        case ZbZclHumiMeasSvrAttrT::ZCL_HUMI_MEAS_ATTR_MIN_MEAS_VAL:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_HUMIDITY, MIN_MEAS_VAL), value);
            break;
        case ZbZclHumiMeasSvrAttrT::ZCL_HUMI_MEAS_ATTR_MAX_MEAS_VAL:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_HUMIDITY, MAX_MEAS_VAL), value);
            break;
        case ZbZclHumiMeasSvrAttrT::ZCL_HUMI_MEAS_ATTR_TOLERANCE:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MEAS_HUMIDITY, TOLERANCE), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_HUMIDITY_MEASUREMENT_ZIGBEE_HPP_ */
