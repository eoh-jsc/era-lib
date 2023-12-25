#ifndef INC_ERA_FROM_MULTISTATE_INPUT_ZIGBEE_HPP_
#define INC_ERA_FROM_MULTISTATE_INPUT_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::multistateInputFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclMultistateAttrT::ZCL_MULTISTATE_BASIC_ATTR_PREVALUE:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(MULTISTATE_INPUT, PREVALUE), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_MULTISTATE_INPUT_ZIGBEE_HPP_ */
