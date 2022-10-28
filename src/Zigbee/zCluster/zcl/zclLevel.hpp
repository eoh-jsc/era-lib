#ifndef INC_ZCL_CLUSTER_LEVEL_HPP_
#define INC_ZCL_CLUSTER_LEVEL_HPP_

/*TODO Level Control*/
/** Level Server Attribute IDs */
enum ZbZclLevelSvrAttrT {
    ZCL_LEVEL_ATTR_CURRLEVEL = 0x0000, /**< CurrentLevel */
    ZCL_LEVEL_ATTR_REMAINTIME = 0x0001, /**< RemainingTime (Optional) */
    ZCL_LEVEL_ATTR_MINLEVEL = 0x0002, /**< MinLevel (Optional) */
    ZCL_LEVEL_ATTR_MAXLEVEL = 0x0003, /**< MaxLevel (Optional) */
    ZCL_LEVEL_ATTR_CURRFREQ = 0x0004, /**< CurrentFrequency (Optional) */
    ZCL_LEVEL_ATTR_MINFREQ = 0x0005, /**< MinFrequency (Optional) */
    ZCL_LEVEL_ATTR_MAXFREQ = 0x0006, /**< MaxFrequency (Optional) */
    ZCL_LEVEL_ATTR_OPTIONS = 0x000F, /**< Options (Optional) */
    ZCL_LEVEL_ATTR_ONOFF_TRANS_TIME = 0x0010, /**< OnOffTransitionTime (Optional) */
    ZCL_LEVEL_ATTR_ONLEVEL = 0x0011, /**< OnLevel (Optional) */
    ZCL_LEVEL_ATTR_ON_TRANS_TIME = 0x0012, /**< OnTransitionTime (Optional) */
    ZCL_LEVEL_ATTR_OFF_TRANS_TIME = 0x0013, /**< OffTransitionTime (Optional) */
    ZCL_LEVEL_ATTR_DEFAULT_MOVE_RATE = 0x0014, /**< DefaultMoveRate (Optional) */
    ZCL_LEVEL_ATTR_STARTUP_CURRLEVEL = 0x4000 /**< StartUpCurrentLevel (Optional) */
};

/* Level Command IDs  */
enum ZbZclLevelSvrCmdT {
    ZCL_LEVEL_COMMAND_MOVELEVEL = 0x00,
    ZCL_LEVEL_COMMAND_MOVE = 0x01,
    ZCL_LEVEL_COMMAND_STEP = 0x02,
    ZCL_LEVEL_COMMAND_STOP = 0x03,
    ZCL_LEVEL_COMMAND_MOVELEVEL_ONOFF = 0x04,
    ZCL_LEVEL_COMMAND_MOVE_ONOFF = 0x05,
    ZCL_LEVEL_COMMAND_STEP_ONOFF = 0x06,
    ZCL_LEVEL_COMMAND_STOP_ONOFF = 0x07,
    ZCL_LEVEL_COMMAND_MOVE_TO_LEVEL_TUYA = 0xF0
};

/* Move mode field */
enum ZbZclLevelModeT {
    ZCL_LEVEL_MODE_UP = 0x00,
    ZCL_LEVEL_MODE_DOWN = 0x01,
    ZCL_LEVEL_MODE_STOP = 0x02
};

#define ZCL_LEVEL_OPTIONS_EXECUTE_IF_OFF            0x01
#define ZCL_LEVEL_OPTIONS_COUPLE_TO_COLOR           0x02

#define ZCL_LEVEL_MINIMUM_LEVEL                     0x00
#define ZCL_LEVEL_MAXIMUM_LEVEL                     0xFE
#define ZCL_LEVEL_ILLEGAL_LEVEL                     0xFF

#define ZCL_LEVEL_MINIMUM_RATE                      0x00
#define ZCL_LEVEL_MAXIMUM_RATE                      0xFE
#define ZCL_LEVEL_ILLEGAL_RATE                      0xFF

#define ZCL_LEVEL_MINIMUM_TRANS_TIME                0x0000
#define ZCL_LEVEL_MAXIMUM_TRANS_TIME                0xFFFE
#define ZCL_LEVEL_ILLEGAL_TRANS_TIME                0xFFFF

#endif /* INC_ZCL_CLUSTER_LEVEL_HPP_ */
