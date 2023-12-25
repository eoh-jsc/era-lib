#ifndef INC_ERA_TO_LEVEL_ZIGBEE_HPP_
#define INC_ERA_TO_LEVEL_ZIGBEE_HPP_

#include <Zigbee/ERaToZigbee.hpp>

template <class Zigbee>
bool ERaToZigbee<Zigbee>::levelToZigbee(const cJSON* const root, const cJSON* const current, AFAddrType_t& dstAddr, const ConvertToZigbeeT type) {
    cJSON* subItem = nullptr;
    uint16_t transition {0};
    bool onOff {false};

    switch (type) {
        case ConvertToZigbeeT::CONVERT_SET_TO_ZIGBEE:
            if (ERaStrNCmp(current->string, "level")) {
                onOff = this->getEndpointToZigbee(current, "level", dstAddr.endpoint);
                subItem = cJSON_GetObjectItem(root, "transition");
                if (cJSON_IsNumber(subItem)) {
                    transition = subItem->valueint * 10;
                }
                if (cJSON_IsNumber(current)) {
                    if (onOff) {
                        CommandZigbee::moveToLevelWithOnOffGenLevelCtrl(dstAddr, current->valueint, transition);
                    }
                    else {
                        CommandZigbee::moveToLevelGenLevelCtrl(dstAddr, current->valueint, transition);
                    }
                }
                else if (cJSON_IsString(current)) {
                    if (ERaStrCmp(current->valuestring, "stop")) {
                        if (onOff) {
                            CommandZigbee::stopWithOnOffGenLevelCtrl(dstAddr);
                        }
                        else {
                            CommandZigbee::stopGenLevelCtrl(dstAddr);
                        }
                    }
                    else if (ERaStrCmp(current->valuestring, "open")) {
                        if (onOff) {
                            CommandZigbee::moveToLevelWithOnOffGenLevelCtrl(dstAddr, 0x64, transition);
                        }
                        else {
                            CommandZigbee::moveToLevelGenLevelCtrl(dstAddr, 0x64, transition);
                        }
                    }
                    else if (ERaStrCmp(current->valuestring, "close")) {
                        if (onOff) {
                            CommandZigbee::moveToLevelWithOnOffGenLevelCtrl(dstAddr, 0x00, transition);
                        }
                        else {
                            CommandZigbee::moveToLevelGenLevelCtrl(dstAddr, 0x00, transition);
                        }
                    }
                }
            }
            break;
        case ConvertToZigbeeT::CONVERT_GET_TO_ZIGBEE:
            if (ERaStrNCmp(current->string, "level")) {
                this->getEndpointToZigbee(current, "level", dstAddr.endpoint);
                if (cJSON_Empty(current)) {
                    CommandZigbee::readAttributes(dstAddr, EndpointListT::ENDPOINT1,
                                                ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, ManufacturerCodesT::MANUF_CODE_NONE,
                                                {ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_CURRLEVEL});
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

#endif /* INC_ERA_TO_LEVEL_ZIGBEE_HPP_ */
