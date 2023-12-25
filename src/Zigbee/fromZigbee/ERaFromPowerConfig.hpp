#ifndef INC_ERA_FROM_POWER_CONFIG_ZIGBEE_HPP_
#define INC_ERA_FROM_POWER_CONFIG_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::powerConfigFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_VOLTAGE), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_PCT), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_SIZE:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_SIZE), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_ALARM_MASK:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_ALARM_MASK), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLT_MIN:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_VOLT_MIN), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD1:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_VTHRESHOLD1), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD2:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_VTHRESHOLD2), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD3:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_VTHRESHOLD3), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT_MIN:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_PCT_MIN), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD1:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_PTHRESHOLD1), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD2:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_PTHRESHOLD2), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD3:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_PTHRESHOLD3), value);
            break;
        case ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_ALARM_STATE:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(POWER_CONFIG, BATTERY_ALARM_STATE), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_POWER_CONFIG_ZIGBEE_HPP_ */
