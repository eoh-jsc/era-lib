#ifndef INC_ERA_FROM_ONOFF_ZIGBEE_HPP_
#define INC_ERA_FROM_ONOFF_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::onOffFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_ONOFF:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(ONOFF, ONOFF), value);
            break;
        case ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_ON_TIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(ONOFF, ON_TIME), value);
            break;
        case ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_OFF_WAIT_TIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(ONOFF, OFF_WAIT_TIME), value);
            break;
        case ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_STARTUP_ONOFF:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(ONOFF, STARTUP_ONOFF), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::onOffSpecificFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, DefaultRsp_t& defaultRsp) {
    if (key == nullptr) {
        return false;
    }

    bool defined {true};
    std::string keyId(key);
    std::transform(keyId.begin(), keyId.end(), keyId.begin(), [](unsigned char c){
        return std::tolower(c);
    });
    uint8_t cmd = afMsg.cmdId;

    defaultRsp.value = cmd;
    defaultRsp.isCmd = true;
    this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(ONOFF, STATE_COMMAND), cmd);

    switch (cmd) {
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_OFF:
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_ON:
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_TOGGLE:
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_OFF_WITH_EFFECT:
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_ON_WITH_RECALL:
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_ON_WITH_TIMED_OFF:
            break;
        case ZbZclOnOffSvrCmdT::ZCL_ONOFF_TUYA_BUTTON:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(ONOFF, TUYA_BUTTON), afMsg.pData[0]);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_ONOFF_ZIGBEE_HPP_ */
