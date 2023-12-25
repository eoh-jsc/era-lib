#ifndef INC_ZCL_CLUSTER_BASIC_HPP_
#define INC_ZCL_CLUSTER_BASIC_HPP_

/*TODO Basic*/
/** Basic Server Attribute IDs */
enum ZbZclBasicSvrAttrT {
    ZCL_BASIC_ATTR_ZCL_VERSION = 0x0000, /**< ZCLVersion */
    ZCL_BASIC_ATTR_APP_VERSION, /**< ApplicationVersion (Optional) */
    ZCL_BASIC_ATTR_STACK_VERSION, /**< StackVersion (Optional) */
    ZCL_BASIC_ATTR_HARDWARE_VERSION, /**< HWVersion (Optional) */
    ZCL_BASIC_ATTR_MFR_NAME, /**< ManufacturerName (Optional) */
    ZCL_BASIC_ATTR_MODEL_NAME, /**< ModelIdentifier (Optional) */
    ZCL_BASIC_ATTR_DATE_CODE, /**< DateCode (Optional) */
    ZCL_BASIC_ATTR_POWER_SOURCE, /**< PowerSource */

    ZCL_BASIC_ATTR_LOCATION = 0x0010, /**< LocationDescription (Optional) */
    ZCL_BASIC_ATTR_ENVIRONMENT, /**< PhysicalEnvironment (Optional) */
    ZCL_BASIC_ATTR_ENABLED, /**< DeviceEnabled (Optional) */
    ZCL_BASIC_ATTR_ALARM_MASK, /**< AlarmMask (Optional) */
    ZCL_BASIC_ATTR_DISABLE_LOCAL_CONFIG, /**< DisableLocalConfig (Optional) */

    ZCL_BASIC_ATTR_SW_BUILD_ID = 0x4000, /**< SWBuildID (Optional) */

    ZCL_BASIC_ATTR_TUYA_TRANSMIT_POWER = 0xFFDE, /**< TransmitPower (Optional) */

    ZCL_BASIC_ATTR_XIAOMI_CURTAIN_OPTIONS = 0x0401, /**< Xiaomi Curtain Options - zcl_charStr - LUMI_UNITED_TECH */
    ZCL_BASIC_ATTR_XIAOMI_DATA = 0xFF01, /**< Xiaomi Data - LUMI_UNITED_TECH */
    ZCL_BASIC_ATTR_XIAOMI_VIBRATION_SENSITIVITY = 0xFF0D, /**< Xiaomi Vibration Sensitivity - zcl_uint8 - LUMI_UNITED_TECH */
    ZCL_BASIC_ATTR_XIAOMI_PREVENT_RESET = 0xFFF0, /**< Xiaomi Prevent Reset - zcl_octetStr - LUMI_UNITED_TECH */

    ZCL_ATTR_CLUSTER_REVISION = 0xFFFD,

    ZCL_BASIC_ATTR_TUYA_UNKNOWN = 0xFFFE
};

/* Power Source Enumerations */
#define ZCL_BASIC_POWER_UNKNOWN                  0x00
#define ZCL_BASIC_POWER_SINGLE_PHASE             0x01
#define ZCL_BASIC_POWER_THREE_PHASE              0x02
#define ZCL_BASIC_POWER_BATTERY                  0x03
#define ZCL_BASIC_POWER_DC                       0x04
#define ZCL_BASIC_POWER_EMERGENCY_CONSTANT       0x05
#define ZCL_BASIC_POWER_EMERGENCY_TRANSFER       0x06
#define ZCL_BASIC_POWER_BATTERY_BACKUP_BIT       0x80

/* Physical Environment Enumerations */
#define ZCL_BASIC_ENVIRONMENT_UNSPECIFIED        0x00
#define ZCL_BASIC_ENVIRONMENT_MIRROR_SUPPORT     0x01
#define ZCL_BASIC_ENVIRONMENT_UNKNOWN            0xff

/* Alarm Codes */
#define ZCL_BASIC_ALARM_CODE_HARDWARE            0x00
#define ZCL_BASIC_ALARM_CODE_SOFTWARE            0x01

/* Alarm Mask */
#define ZCL_BASIC_ALARM_MASK_ALL                 0x03U
#define ZCL_BASIC_ALARM_MASK_HARDWARE            0x01U
#define ZCL_BASIC_ALARM_MASK_SOFTWARE            0x02U

/* Only for the ZCL Basic 'ZCLVersion' attribute. This version attribute is unrelated to the ZCL Global Revision Attribute. */
enum ZbZclBasicZclVerT {
    ZCL_BASIC_ZCL_VERSION_6 = 0x02U,
    ZCL_BASIC_ZCL_VERSION_7 = 0x03U,
    ZCL_BASIC_ZCL_VERSION_8 = 0x08U /* Starting from ZCL8, this number matches the revision of ZCL. */
};

/* Local Config */
#define ZCL_BASIC_LOCAL_CONFIG_RESET_DISABLE     0x01U
#define ZCL_BASIC_LOCAL_CONFIG_DEVICE_DISABLE    0x02U

/* Commands */
enum ZbZclBasicSvrCmdT {
    ZCL_BASIC_RESET_FACTORY = 0x00,
    ZCL_BASIC_TUYA_SETUP = 0xF0
};

#endif /* INC_ZCL_CLUSTER_BASIC_HPP_ */
