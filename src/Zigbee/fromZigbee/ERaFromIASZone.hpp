#ifndef INC_ERA_FROM_IAS_ZONE_ZIGBEE_HPP_
#define INC_ERA_FROM_IAS_ZONE_ZIGBEE_HPP_

#include <Zigbee/ERaFromZigbee.hpp>

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::iasZoneFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, uint16_t attribute, uint64_t& value) {
    bool defined {true};
    switch (attribute) {
        case ZbZclIasZoneServerAttrT::ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATE:
            switch (value) {
                case ZbZclIasZoneServerZoneStateT::ZCL_IAS_ZONE_SVR_STATE_NOT_ENROLLED:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_STATE), "not_enrolled");
                    break;
                case ZbZclIasZoneServerZoneStateT::ZCL_IAS_ZONE_SVR_STATE_ENROLLED:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_STATE), "enrolled");
                    break;
                default:
                    this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, ZONE_STATE), value);
                    break;
            }
            break;
        case ZbZclIasZoneServerAttrT::ZCL_IAS_ZONE_SVR_ATTR_ZONE_TYPE:
            switch (value) {
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_STANDARD_CIE:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "standard_cie");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_MOTION_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "motion_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_CONTACT_SWITCH:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "contact_switch");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_DOOR_WINDOW:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "door_window");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_FIRE_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "fire_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_WATER_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "water_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_CO_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "co_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_PERSONAL_EMERG_DEVICE:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "personal_emergency_device");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_MOVEMENT_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "movement_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_REMOTE_CONTROL:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "remote_control");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_KEY_FOB:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "key_fob");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_KEYPAD:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "keypad");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_STANDARD_WARNING_DEVICE:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "standard_warning_device");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_GLASS_SENSOR:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "glass_sensor");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_SECURITY_REPEATER:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "security_repeater");
                    break;
                case ZbZclIasZoneServerZoneTypeT::ZCL_IAS_ZONE_SVR_ZONE_TYPE_INVALID:
                    this->addDataZigbee(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), "invalid");
                    break;
                default:
                    this->addDataZigbee<uint16_t>(afMsg, root, KEY(IAS_ZONE, ZONE_TYPE), value);
                    break;
            }
            break;
        case ZbZclIasZoneServerAttrT::ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATUS:
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(IAS_ZONE, ZONE_STATUS), value);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, ALARM_STATE), value & 0x03);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TAMPER), (value >> 2) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, BATTERY_LOW), (value >> 3) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, SUPERVISION), (value >> 4) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, RESTORE), (value >> 5) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TROUBLE), (value >> 6) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, AC_STATUS), (value >> 7) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TEST_MODE), (value >> 8) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, BATTERY_DEFECT), (value >> 9) & 0x01);
            break;
        case ZbZclIasZoneServerAttrT::ZCL_IAS_ZONE_SVR_ATTR_CURRENT_ZONE_SENSITIVITY_LEVEL:
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, SENSITIVITY_LEVEL), value);
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

template <class Zigbee>
bool ERaFromZigbee<Zigbee>::iasZoneSpecificFromZigbee(const DataAFMsg_t& afMsg, cJSON* root, const char* key, DefaultRsp_t& defaultRsp) {
    if (key == nullptr) {
        return false;
    }

    bool defined {true};
    std::string keyId(key);
    std::transform(keyId.begin(), keyId.end(), keyId.begin(), [](unsigned char c){
        return std::tolower(c);
    });
    uint8_t cmd = afMsg.cmdId;

    this->removeDataZigbee(afMsg, root, KEY_SPEC(IAS_ZONE, ENROLL_REQ));

    switch (cmd) {
        case ZbZclIasZoneServerCommandsT::ZCL_IAS_ZONE_SVR_CMD_ZONE_STATUS_CHANGE_NOTIFY: {
            uint16_t zoneStatus = BUILD_UINT16(afMsg.pData[0]);
            this->addDataZigbee<uint16_t>(afMsg, root, KEY(IAS_ZONE, ZONE_STATUS), zoneStatus);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, ALARM_STATE), zoneStatus & 0x03);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TAMPER), (zoneStatus >> 2) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, BATTERY_LOW), (zoneStatus >> 3) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, SUPERVISION), (zoneStatus >> 4) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, RESTORE), (zoneStatus >> 5) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TROUBLE), (zoneStatus >> 6) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, AC_STATUS), (zoneStatus >> 7) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, TEST_MODE), (zoneStatus >> 8) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY(IAS_ZONE, BATTERY_DEFECT), (zoneStatus >> 9) & 0x01);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(IAS_ZONE, EXT_ZONE_STATUS), afMsg.pData[2]);
            this->addDataZigbee<uint8_t>(afMsg, root, KEY_SPEC(IAS_ZONE, ZONE_ID), afMsg.pData[3]);
            this->addDataZigbee<uint16_t>(afMsg, root, KEY_SPEC(IAS_ZONE, DELAY), BUILD_UINT16(afMsg.pData[4]));
            defaultRsp.value = zoneStatus & 0x03;
        }
            break;
        case ZbZclIasZoneServerCommandsT::ZCL_IAS_ZONE_SVR_CMD_ZONE_ENROLL_REQUEST: {
            cJSON* item = cJSON_CreateObject();
            if (item == nullptr) {
                break;
            }
            cJSON_AddNumberToObject(item, KEY_SPEC(IAS_ZONE, ZONE_TYPE), BUILD_UINT16(afMsg.pData[0]));
            this->addDataZigbee(afMsg, root, KEY_SPEC(IAS_ZONE, ENROLL_REQ), item);
        }
            break;
        default:
            defined = false;
            break;
    }
    return defined;
}

#endif /* INC_ERA_FROM_IAS_ZONE_ZIGBEE_HPP_ */
