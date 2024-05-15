#ifndef INC_ZCL_CLUSTER_IAS_HPP_
#define INC_ZCL_CLUSTER_IAS_HPP_

/*TODO IAS Zone*/
/** IAS Zone Server Attribute IDs */
enum ZbZclIasZoneServerAttrT {
    /* Zone Information Set */
    ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATE = 0x0000, /**< ZoneState */
    ZCL_IAS_ZONE_SVR_ATTR_ZONE_TYPE = 0x0001, /**< ZoneType */
    ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATUS = 0x0002, /**< ZoneStatus */ /*8 bits - zclstatus, map16 - zone status, map8 - extend status, uint8 - zone id, uint16 - delay*/
    /* Zone Settings Set */
    ZCL_IAS_ZONE_SVR_ATTR_CIE_ADDR = 0x0010, /**< IAS_CIE_Address */
    ZCL_IAS_ZONE_SVR_ATTR_ZONE_ID = 0x0011, /**< ZoneID */
    ZCL_IAS_ZONE_SVR_ATTR_NUM_ZONE_SENSITIVITY_SUPPORTED = 0x0012, /**< NumberOfZoneSensitivityLevelsSupported (Optional) */
    ZCL_IAS_ZONE_SVR_ATTR_CURRENT_ZONE_SENSITIVITY_LEVEL = 0x0013, /**< CurrentZoneSensitivityLevel (Optional) */

    /* Exegin add-on (internal) to preserve endpoint of CIE */
    ZCL_IAS_ZONE_SVR_ATTR_CIE_ENDPOINT = 0x7fff, /**< Exegin internal (Optional) */
};

/* IAS Zone Server commands/responses */
enum ZbZclIasZoneServerCommandsT {
    ZCL_IAS_ZONE_SVR_CMD_ZONE_STATUS_CHANGE_NOTIFY = 0x00, /* Zone Status Change Notification */
    ZCL_IAS_ZONE_SVR_CMD_ZONE_ENROLL_REQUEST = 0x01, /* Zone Enroll Request */
};

/* IAS Zone Client commands/responses */
enum ZbZclIasZoneClientCommandsT {
    ZCL_IAS_ZONE_CLI_CMD_ZONE_ENROLL_RESPONSE = 0x00, /* Zone Enroll Response */
    ZCL_IAS_ZONE_CLI_CMD_INITIATE_NORMAL_MODE = 0x01, /* Initiate Normal Operation Mode (Optional) */
    ZCL_IAS_ZONE_CLI_CMD_INITIATE_TEST_MODE = 0x02, /* Initiate Test Mode (Optional) */
};

/** IAS Zone ZoneState Attribute */
enum ZbZclIasZoneServerZoneStateT {
    ZCL_IAS_ZONE_SVR_STATE_NOT_ENROLLED = 0x00, /**< Not enrolled */
    ZCL_IAS_ZONE_SVR_STATE_ENROLLED = 0x01,
    /**< Enrolled (the client will react to Zone State Change Notification commands from the server) */
};

/** IAS Zone ZoneType Attribute */
enum ZbZclIasZoneServerZoneTypeT {
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_STANDARD_CIE = 0x0000, /**< Standard CIE */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_MOTION_SENSOR = 0x000d, /**< Motion sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_CONTACT_SWITCH = 0x0015, /**< Contact switch */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_DOOR_WINDOW = 0x0016, /**< Door/Window handle */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_FIRE_SENSOR = 0x0028, /**< Fire sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_WATER_SENSOR = 0x002a, /**< Water sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_CO_SENSOR = 0x002b, /**< Carbon Monoxide (CO) sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_PERSONAL_EMERG_DEVICE = 0x002c, /**< Personal emergency device */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_MOVEMENT_SENSOR = 0x002d, /**< Vibration/Movement sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_REMOTE_CONTROL = 0x010f, /**< Remote Control */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_KEY_FOB = 0x0115, /**< Key fob */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_KEYPAD = 0x021d, /**< Keypad */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_STANDARD_WARNING_DEVICE = 0x0225, /**< Standard Warning Device (see [N1] part 4) */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_GLASS_SENSOR = 0x0226, /**< Glass break sensor */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_SECURITY_REPEATER = 0x0229, /**< Security repeater */
    ZCL_IAS_ZONE_SVR_ZONE_TYPE_INVALID = 0xffff /**< Invalid Zone Type */
};

/** IAS Zone ZoneStatus Attribute */
enum ZbZclIasZoneServerZoneStatusT {
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_ALARM1 = 1 << 0, /**< Alarm1 */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_ALARM2 = 1 << 1, /**< Alarm2 */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_TAMPER = 1 << 2, /**< Tamper */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_BATTERY = 1 << 3, /**< Battery */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_SUPERVISION_REPORTS = 1 << 4, /**< Supervision Notify */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_RESTORE_REPORTS = 1 << 5, /**< Restore Notify */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_TROUBLE = 1 << 6, /**< Trouble */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_AC_MAINS = 1 << 7, /**< AC (mains) */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_TEST = 1 << 8, /**< Test */
    ZCL_IAS_ZONE_SVR_ZONE_STATUS_BATTERY_DEFECT = 1 << 9, /**< Battery Defect */
};
#define ZCL_IAS_ZONE_SVR_ZONE_STATUS_MASK           0x03ffU

/** IAS Zone ZoneStatus Attribute Bit Test Value */
enum ZbZclIasZoneServerModeT {
    ZCL_IAS_ZONE_SVR_MODE_NORMAL = 0, /**< Normal */
    ZCL_IAS_ZONE_SVR_MODE_TEST, /**< Set */
};

/** IAS Zone Enroll Response Code */
enum ZbZclIasZoneClientResponseCodeT {
    ZCL_IAS_ZONE_CLI_RESP_SUCCESS = 0x00, /**< Success */
    ZCL_IAS_ZONE_CLI_RESP_NOT_SUPPORTED = 0x01, /**< Not supported */
    ZCL_IAS_ZONE_CLI_RESP_NO_ENROLL_PERMIT = 0x02, /**< No enroll permit */
    ZCL_IAS_ZONE_CLI_RESP_TOO_MANY_ZONES = 0x03, /**< Too many zones */
};

/*TODO IAS Warning*/
/** IAS Warning Device Server Attribute IDs */
enum ZbZclIasWdSvrAttrT {
    ZCL_IAS_WD_SVR_ATTR_MAX_DURATION = 0x0000, /**< MaxDuration */
};

/* IAS Warning Device Client Generated Commands */
enum ZbZclIasWdCliCommandsT {
    ZCL_IAS_WD_CLI_CMD_START_WARNING = 0x00,
    ZCL_IAS_WD_CLI_CMD_SQUAWK = 0x01,
};

/** IAS Warning Device Warning Modes */
enum ZbZclIasWdWarningModeT {
    ZCL_IAS_WD_WARNING_MODE_STOP = 0, /**< Stop (no warning) */
    ZCL_IAS_WD_WARNING_MODE_BURGLAR = 1, /**< Burglar */
    ZCL_IAS_WD_WARNING_MODE_FIRE = 2, /**< Fire */
    ZCL_IAS_WD_WARNING_MODE_EMERGENCY = 3, /**< Emergency */
    ZCL_IAS_WD_WARNING_MODE_POLICE_PANIC = 4, /**< Police panic */
    ZCL_IAS_WD_WARNING_MODE_FIRE_PANIC = 5, /**< Fire panic */
    ZCL_IAS_WD_WARNING_MODE_EMERGENCY_PANIC = 6, /**< Emergency Panic (i.e., medical issue) */
};

/** IAS Warning Device Strobe Field */
enum ZbZclIasWdStrobeT {
    ZCL_IAS_WD_STROBE_OFF = 0, /**< No strobe */
    ZCL_IAS_WD_STROBE_ON = 1, /**< Use strobe in parallel to warning */
};

/** IAS Warning Device Siren Level Field Values */
enum ZbZclIasWdLevelT {
    ZCL_IAS_WD_LEVEL_LOW = 0, /**< Low level sound */
    ZCL_IAS_WD_LEVEL_MEDIUM = 1, /**< Medium level sound */
    ZCL_IAS_WD_LEVEL_HIGH = 2, /**< High level sound */
    ZCL_IAS_WD_LEVEL_VERY_HIGH = 3, /**< Very high level sound */
};

/** IAS Warning Device Squawk Mode Field */
enum ZbZclIasWdSquawkModeT {
    ZCL_IAS_WD_SQUAWK_MODE_ARMED = 0, /**< Notification sound for “System is armed” */
    ZCL_IAS_WD_SQUAWK_MODE_DISARMED = 1, /**< Notification sound for "System is disarmed" */
};

/*TODO IAS Ace*/
/* Server Generated Commands */
enum ZbZclIasAceServerCommandsT {
    ZCL_IAS_ACE_SVR_CMD_ARM_RSP = 0x00,
    ZCL_IAS_ACE_SVR_CMD_GET_ZONE_ID_MAP_RSP = 0x01,
    ZCL_IAS_ACE_SVR_CMD_GET_ZONE_INFO_RSP = 0x02,
    ZCL_IAS_ACE_SVR_CMD_ZONE_STATUS_CHANGED = 0x03,
    ZCL_IAS_ACE_SVR_CMD_PANEL_STATUS_CHANGED = 0x04,
    ZCL_IAS_ACE_SVR_CMD_GET_PANEL_STATUS_RSP = 0x05,
    ZCL_IAS_ACE_SVR_CMD_SET_BYPASSED_ZONE_LIST = 0x06,
    ZCL_IAS_ACE_SVR_CMD_BYPASS_RSP = 0x07,
    ZCL_IAS_ACE_SVR_CMD_GET_ZONE_STATUS_RSP = 0x08,
};

/* Client Generated Commands */
enum ZbZclIasAceClientCommandsT {
    ZCL_IAS_ACE_CLI_CMD_ARM = 0x00,
    ZCL_IAS_ACE_CLI_CMD_BYPASS = 0x01,
    ZCL_IAS_ACE_CLI_CMD_EMERGENCY = 0x02,
    ZCL_IAS_ACE_CLI_CMD_FIRE = 0x03,
    ZCL_IAS_ACE_CLI_CMD_PANIC = 0x04,
    ZCL_IAS_ACE_CLI_CMD_GET_ZONE_ID_MAP = 0x05,
    ZCL_IAS_ACE_CLI_CMD_GET_ZONE_INFO = 0x06,
    ZCL_IAS_ACE_CLI_CMD_GET_PANEL_STATUS = 0x07,
    ZCL_IAS_ACE_CLI_CMD_GET_BYPASSED_ZONE_LIST = 0x08,
    ZCL_IAS_ACE_CLI_CMD_GET_ZONE_STATUS = 0x09,
};

enum ZbZclIasAceArmModeT {
    ZCL_IAS_ACE_ARM_MODE_DISARM = 0x00,
    ZCL_IAS_ACE_ARM_MODE_ARM_DAY_ZONES = 0x01,
    ZCL_IAS_ACE_ARM_MODE_ARM_NIGHT_ZONES = 0x02,
    ZCL_IAS_ACE_ARM_MODE_ARM_ALL_ZONES = 0x03,
};

enum ZbZclIasAceArmNotifyT {
    ZCL_IAS_ACE_ARM_NOTIFY_ALL_ZONES_DISARMED = 0x00,
    ZCL_IAS_ACE_ARM_NOTIFY_ONLY_DAY_ZONES_ARMED = 0x01,
    ZCL_IAS_ACE_ARM_NOTIFY_ONLY_NIGHT_ZONES_ARMED = 0x02,
    ZCL_IAS_ACE_ARM_NOTIFY_ALL_ZONES_ARMED = 0x03,
    ZCL_IAS_ACE_ARM_NOTIFY_INVALID_ARM_CODE = 0x04,
    ZCL_IAS_ACE_ARM_NOTIFY_NOT_READY_TO_ARM = 0x05,
    ZCL_IAS_ACE_ARM_NOTIFY_ALREADY_DISARMED = 0x06,
};

enum ZbZclIasAcePanelStatusT {
    ZCL_IAS_ACE_PANEL_STATUS_PANEL_DISARMED = 0x00,
    ZCL_IAS_ACE_PANEL_STATUS_ARMED_STAY = 0x01,
    ZCL_IAS_ACE_PANEL_STATUS_ARMED_NIGHT = 0x02,
    ZCL_IAS_ACE_PANEL_STATUS_ARMED_AWAY = 0x03,
    ZCL_IAS_ACE_PANEL_STATUS_EXIT_DELAY = 0x04,
    ZCL_IAS_ACE_PANEL_STATUS_ENTRY_DELAY = 0x05,
    ZCL_IAS_ACE_PANEL_STATUS_NOT_READY_TO_ARM = 0x06,
    ZCL_IAS_ACE_PANEL_STATUS_IN_ALARM = 0x07,
    ZCL_IAS_ACE_PANEL_STATUS_ARMING_STAY = 0x08,
    ZCL_IAS_ACE_PANEL_STATUS_ARMING_NIGHT = 0x09,
    ZCL_IAS_ACE_PANEL_STATUS_ARMING_AWAY = 0x0a,
};

enum ZbZclIasAceAudibleNotifyT {
    ZCL_IAS_ACE_AUDIBLE_NOTIFY_MUTE = 0x00,
    ZCL_IAS_ACE_AUDIBLE_NOTIFY_DEFAULT_SOUND = 0x01,
};

enum ZbZclIasAceAlarmStatusT {
    ZCL_IAS_ACE_ALARM_STATUS_NO_ALARM = 0x00,
    ZCL_IAS_ACE_ALARM_STATUS_BURGLAR = 0x01,
    ZCL_IAS_ACE_ALARM_STATUS_FIRE = 0x02,
    ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY = 0x03,
    ZCL_IAS_ACE_ALARM_STATUS_POLICE_PANIC = 0x04,
    ZCL_IAS_ACE_ALARM_STATUS_FIRE_PANIC = 0x05,
    ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY_PANIC = 0x06,
};

enum ZbZclIasAceBypassResultT {
    ZCL_IAS_ACE_BYPASS_RESULT_ZONE_BYPASSED = 0x00,
    ZCL_IAS_ACE_BYPASS_RESULT_ZONE_NOT_BYPASSED = 0x01,
    ZCL_IAS_ACE_BYPASS_RESULT_NOT_ALLOWED = 0x02,
    ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ZONE_ID = 0x03,
    ZCL_IAS_ACE_BYPASS_RESULT_UNKNOWN_ZONE_ID = 0x04,
    ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ARM_CODE = 0x05,
};

enum ZbZclIasAceBypassPermsT {
    ZCL_IAS_ACE_BYPASS_PERMS_ALLOWED = 0x00,
    ZCL_IAS_ACE_BYPASS_PERMS_NOT_ALLOWED = 0x01,
};

#endif /* INC_ZCL_CLUSTER_IAS_HPP_ */