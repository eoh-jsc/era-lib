#ifndef INC_ZCL_CLUSTER_ONOFF_HPP_
#define INC_ZCL_CLUSTER_ONOFF_HPP_

/*TODO Onoff*/
/** OnOff Server Attribute IDs */
enum ZbZclOnOffSvrAttrT {
    ZCL_ONOFF_ATTR_ONOFF = 0x0000, /**< OnOff - bool*/
    ZCL_ONOFF_ATTR_GLOBAL_SCENE_CONTROL = 0x4000, /**< GlobalSceneControl (Optional) - bool*/
    ZCL_ONOFF_ATTR_ON_TIME, /**< OnTime (Optional) - uint16*/
    ZCL_ONOFF_ATTR_OFF_WAIT_TIME, /**< OffWaitTime (Optional) - uint16*/
    ZCL_ONOFF_ATTR_STARTUP_ONOFF, /**< StartUpOnOff (Optional) - enum8*/

    ZCL_ONOFF_ATTR_TUYA_BLACKLIGHT_SWITCH = 0x5000, /**< TuyaBacklightSwitch (Optional) - enum8 */

    ZCL_ONOFF_ATTR_TUYA_BLACKLIGHT_MODE = 0x8001, /**< TuyaBacklightMode (Optional) - enum8 */
    ZCL_ONOFF_ATTR_MOES_STARTUP_ONOFF, /**< MoesStartUpOnOff (Optional) - enum8*/
    
    ZCL_ONOFF_ATTR_TUYA_OPERATION_MODE = 0x8004, /**< TuyaOperationMode (Optional) - enum8 */

    ZCL_ONOFF_ATTR_ELKO_PRE_WARNING_TIME = 0xE000, /**< ElkoPreWarningTime (Optional) - uint16 code ELKO*/
    ZCL_ONOFF_ATTR_ELKO_ON_TIME_RELOAD, /**< ElkoOnTimeReload (Optional) - uint32 code ELKO*/
    ZCL_ONOFF_ATTR_ELKO_ON_TIME_RELOAD_OPTION /**< ElkoOnTimeReloadOptions (Optional) - bitmap8 code ELKO*/
};

/* OnOff Command IDs (don't include in doxygen) */
enum ZbZclOnOffSvrCmdT {
    ZCL_ONOFF_COMMAND_OFF = 0x00,
    ZCL_ONOFF_COMMAND_ON = 0x01,
    ZCL_ONOFF_COMMAND_TOGGLE = 0x02,
    ZCL_ONOFF_OFF_WITH_EFFECT = 0x40,
    ZCL_ONOFF_ON_WITH_RECALL = 0x41,/*on with recall global scene*/
    ZCL_ONOFF_ON_WITH_TIMED_OFF = 0x42,
    ZCL_ONOFF_TUYA_BUTTON = 0xFD
};

#endif /* INC_ZCL_CLUSTER_ONOFF_HPP_ */
