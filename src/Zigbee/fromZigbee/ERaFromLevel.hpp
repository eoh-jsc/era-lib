#ifndef INC_ERA_FROM_LEVEL_ZIGBEE_HPP_
#define INC_ERA_FROM_LEVEL_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::levelFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_CURRLEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, CURRLEVEL), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_REMAINTIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(LEVEL, REMAINTIME), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_MINLEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, MINLEVEL), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_MAXLEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, MAXLEVEL), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_ONOFF_TRANS_TIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(LEVEL, ONOFF_TRANS_TIME), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_ONLEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, ONLEVEL), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_ON_TRANS_TIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(LEVEL, ON_TRANS_TIME), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_OFF_TRANS_TIME:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(LEVEL, OFF_TRANS_TIME), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_DEFAULT_MOVE_RATE:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, DEFAULT_MOVE_RATE), value);
            break;
        case ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_STARTUP_CURRLEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(LEVEL, STARTUP_CURRLEVEL), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::levelSpecificFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, DefaultRsp_t& defaultRsp) {
    if (key == nullptr) {
        return false;
    }

    bool defined {true};
    std::string keyId(key);
    std::transform(keyId.begin(), keyId.end(), keyId.begin(), [](unsigned char c){
        return std::tolower(c);
    });
    uint8_t cmd = afMsg.cmdId;

    this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(LEVEL, LEVEL_COMMAND), cmd);
    this->removeDataZigbee(afMsg, root, KEY_SPEC(LEVEL, MOVELEVEL));
    this->removeDataZigbee(afMsg, root, KEY_SPEC(LEVEL, MOVELEVEL_ONOFF));

    switch (cmd) {
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVELEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(LEVEL, MOVELEVEL), afMsg.pData[0]);
            defaultRsp.value = afMsg.pData[0];
            break;
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVE:
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STEP:
            break;
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STOP:
            defaultRsp.value = cmd;
            defaultRsp.isCmd = true;
            break;
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVELEVEL_ONOFF:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(LEVEL, MOVELEVEL_ONOFF), afMsg.pData[0]);
            defaultRsp.value = afMsg.pData[0];
            break;
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_MOVE_ONOFF:
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STEP_ONOFF:
        case ZbZclLevelSvrCmdT::ZCL_LEVEL_COMMAND_STOP_ONOFF:
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_LEVEL_ZIGBEE_HPP_ */
