#ifndef INC_ERA_TO_ONOFF_ZIGBEE_HPP_
#define INC_ERA_TO_ONOFF_ZIGBEE_HPP_

#include <Zigbee/ERaToZigbee.hpp>

template <class Zigbee>
bool ERaToZigbee<Zigbee>::stateToZigbee(const cJSON* const root, const cJSON* const current, AFAddrType_t& dstAddr, const ConvertToZigbeeT type) {
    cJSON* subItem = nullptr;
    uint16_t transition {0};
    uint8_t controlBits {0};
    uint16_t onTime {0};
    uint16_t offWaitTime {0};
    bool isSpecific {false};
    bool isOnWithTimedOff {false};

    switch (type) {
        case ConvertToZigbeeT::CONVERT_SET_TO_ZIGBEE:
            if (CompareNString(current->string, "state")) {
                this->getEndpointToZigbee(current, "state", dstAddr.endpoint);
                subItem = cJSON_GetObjectItem(root, "is_specific");
                if (cJSON_IsBool(subItem) || cJSON_IsNumber(subItem)) {
                    isSpecific = subItem->valueint;
                }
                subItem = cJSON_GetObjectItem(root, "control_bit");
                if (cJSON_IsNumber(subItem)) {
                    controlBits = subItem->valueint;
                }
                subItem = cJSON_GetObjectItem(root, "on_time");
                if (cJSON_IsNumber(subItem)) {
                    isOnWithTimedOff = true;
                    onTime = subItem->valueint * (isSpecific ? 1 : 10);
                }
                subItem = cJSON_GetObjectItem(root, "off_wait_time");
                if (cJSON_IsNumber(subItem))    {
                    isOnWithTimedOff = true;
                    offWaitTime = subItem->valueint * (isSpecific ? 1 : 10);
                }
                if (cJSON_IsNumber(current)) {
                    if (current->valueint == ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_ON && isOnWithTimedOff) {
                        CommandZigbee::onWithTimeOffGenOnOff(dstAddr, controlBits, onTime, offWaitTime);
                    }
                    else {
                        CommandZigbee::onoffGenOnOff(dstAddr, current->valueint);
                    }
                }
                else if (cJSON_IsString(current)) {
                    if (ERaStrCmp(current->valuestring, "on")) {
                        if (isOnWithTimedOff) {
                            CommandZigbee::onWithTimeOffGenOnOff(dstAddr, controlBits, onTime, offWaitTime);
                        }
                        else {
                            CommandZigbee::onoffGenOnOff(dstAddr, ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_ON);
                        }
                    }
                    else if (ERaStrCmp(current->valuestring, "off")) {
                        CommandZigbee::onoffGenOnOff(dstAddr, ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_OFF);
                    }
                    else if (ERaStrCmp(current->valuestring, "toggle")) {
                        CommandZigbee::onoffGenOnOff(dstAddr, ZbZclOnOffSvrCmdT::ZCL_ONOFF_COMMAND_TOGGLE);
                    }
                    else {
                        subItem = cJSON_GetObjectItem(root, "transition");
                        if (cJSON_IsNumber(subItem)) {
                            transition = subItem->valueint * 10;
                        }
                        if (ERaStrCmp(current->valuestring, "stop")) {
                            CommandZigbee::stopWithOnOffGenLevelCtrl(dstAddr);
                        }
                        else if (ERaStrCmp(current->valuestring, "open")) {
                            CommandZigbee::moveToLevelWithOnOffGenLevelCtrl(dstAddr, 0x64, transition);
                        }
                        else if (ERaStrCmp(current->valuestring, "close")) {
                            CommandZigbee::moveToLevelWithOnOffGenLevelCtrl(dstAddr, 0x00, transition);
                        }
                    }
                }
                return true;
            }
            break;
        case ConvertToZigbeeT::CONVERT_GET_TO_ZIGBEE:
            if (CompareNString(current->string, "state")) {
                this->getEndpointToZigbee(current, "state", dstAddr.endpoint);
                if (cJSON_Empty(current)) {
                    CommandZigbee::readAttributes(dstAddr, EndpointListT::ENDPOINT1,
                                                ClusterIDT::ZCL_CLUSTER_ONOFF, ManufacturerCodesT::MANUF_CODE_NONE,
                                                {ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_ONOFF});
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

#endif /* INC_ERA_TO_ONOFF_ZIGBEE_HPP_ */
