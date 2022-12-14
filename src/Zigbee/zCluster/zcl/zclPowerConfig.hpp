#ifndef INC_ZCL_CLUSTER_POWER_CONFIG_HPP_
#define INC_ZCL_CLUSTER_POWER_CONFIG_HPP_

/*TODO Power Configuration*/
/** Power Configuration Server Attributes IDs */
enum ZbZclPowerConfigSvrAttrT {
    ZCL_POWER_CONFIG_ATTR_MAINS_VOLTAGE = 0x0000, /**< MainsVoltage (Optional) */
    ZCL_POWER_CONFIG_ATTR_MAINS_FREQ = 0x0001, /**< MainsFrequency (Optional) */
    ZCL_POWER_CONFIG_ATTR_MAINS_ALARM_MASK = 0x0010, /**< MainsAlarmMask (Optional) */
    ZCL_POWER_CONFIG_ATTR_MAINS_VOLT_MIN = 0x0011, /**< MainsVoltageMinThreshold (Optional) */
    ZCL_POWER_CONFIG_ATTR_MAINS_VOLT_MAX = 0x0012, /**< MainsVoltageMaxThreshold (Optional) */
    ZCL_POWER_CONFIG_ATTR_MAINS_VOLT_DWELL = 0x0013, /**< MainsVoltageDwellTripPoint (Optional) */
    /* First battery */
    ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE = 0x0020, /**< BatteryVoltage (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_PCT = 0x0021, /**< BatteryPercentageRemaining (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_MFR_NAME = 0x0030, /**< BatteryManufacturer (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_SIZE = 0x0031, /**< BatterySize (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_AHRRATING = 0x0032, /**< BatteryAHrRating (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_QUANTITY = 0x0033, /**< BatteryQuantity (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_RATED_VOLT = 0x0034, /**< BatteryRatedVoltage (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_ALARM_MASK = 0x0035, /**< BatteryAlarmMask (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_VOLT_MIN = 0x0036, /**< BatteryVoltageMinThreshold (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD1 = 0x0037, /**< BatteryVoltageThreshold1 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD2 = 0x0038, /**< BatteryVoltageThreshold2 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_VTHRESHOLD3 = 0x0039, /**< BatteryVoltageThreshold3 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_PCT_MIN = 0x003A, /**< BatteryPercentageMinThreshold (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD1 = 0x003B, /**< BatteryPercentageThreshold1 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD2 = 0x003C, /**< BatteryPercentageThreshold2 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_PTHRESHOLD3 = 0x003D, /**< BatteryPercentageThreshold3 (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY_ALARM_STATE = 0x003E, /**< BatteryAlarmState (Optional) */
    /* Second battery */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_VOLTAGE = 0x0040, /**< BatteryVoltage - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_PCT = 0x0041, /**< BatteryPercentageRemaining - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_MFR_NAME = 0x0050, /**< BatteryManufacturer - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_SIZE = 0x0051, /**< BatterySize - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_AHRRATING = 0x0052, /**< BatteryAHrRating - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_QUANTITY = 0x0053, /**< BatteryQuantity - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_RATED_VOLT = 0x0054, /**< BatteryRatedVoltage - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_ALARM_MASK = 0x0055, /**< BatteryAlarmMask - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_VOLT_MIN = 0x0056, /**< BatteryVoltageMinThreshold - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_VTHRESHOLD1 = 0x0057, /**< BatteryVoltageThreshold1 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_VTHRESHOLD2 = 0x0058, /**< BatteryVoltageThreshold2 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_VTHRESHOLD3 = 0x0059, /**< BatteryVoltageThreshold3 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_PCT_MIN = 0x005A, /**< BatteryPercentageMinThreshold - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_PTHRESHOLD1 = 0x005B, /**< BatteryPercentageThreshold1 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_PTHRESHOLD2 = 0x005C, /**< BatteryPercentageThreshold2 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_PTHRESHOLD3 = 0x005D, /**< BatteryPercentageThreshold3 - Second battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY2_ALARM_STATE = 0x005E, /**< BatteryAlarmState - Second battery (Optional) */
    /* Third battery */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_VOLTAGE = 0x0060, /**< BatteryVoltage - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_PCT = 0x0061, /**< BatteryPercentageRemaining - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_MFR_NAME = 0x0070, /**< BatteryManufacturer - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_SIZE = 0x0071, /**< BatterySize - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_AHRRATING = 0x0072, /**< BatteryAHrRating - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_QUANTITY = 0x0073, /**< BatteryQuantity - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_RATED_VOLT = 0x0074, /**< BatteryRatedVoltage - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_ALARM_MASK = 0x0075, /**< BatteryAlarmMask - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_VOLT_MIN = 0x0076, /**< BatteryVoltageMinThreshold - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_VTHRESHOLD1 = 0x0077, /**< BatteryVoltageThreshold1 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_VTHRESHOLD2 = 0x0078, /**< BatteryVoltageThreshold2 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_VTHRESHOLD3 = 0x0079, /**< BatteryVoltageThreshold3 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_PCT_MIN = 0x007A, /**< BatteryPercentageMinThreshold - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_PTHRESHOLD1 = 0x007B, /**< BatteryPercentageThreshold1 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_PTHRESHOLD2 = 0x007C, /**< BatteryPercentageThreshold2 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_PTHRESHOLD3 = 0x007D, /**< BatteryPercentageThreshold3 - Third Battery (Optional) */
    ZCL_POWER_CONFIG_ATTR_BATTERY3_ALARM_STATE = 0x007E, /**< BatteryAlarmState - Third Battery (Optional) */
};

#define ZCL_POWER_CONFIG_MAINS_ALARM_MASK_MAINS_LOW         0x01U
#define ZCL_POWER_CONFIG_MAINS_ALARM_MASK_MAINS_HIGH        0x02U
#define ZCL_POWER_CONFIG_MAINS_ALARM_MASK_MAINS_LOST        0x04U
#define ZCL_POWER_CONFIG_MAINS_ALARM_MASK_MAINS_MASK        0x07U

#define ZCL_POWER_CONFIG_BATTERY_SIZE_NO_BATTERY            0x00U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_BUILT_IN              0x01U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_OTHER                 0x02U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_AA                    0x03U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_AAA                   0x04U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_C                     0x05U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_D                     0x06U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_CR2                   0x07U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_CR123A                0x08U
#define ZCL_POWER_CONFIG_BATTERY_SIZE_UNKNOWN               0xffU

#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_LOW             0x01U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_BATT_ALARM1     0x02U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_BATT_ALARM2     0x04U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_BATT_ALARM3     0x08U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_MASK            0x0fU

/* Battery Source 1 Alarm State */
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_1_MIN_THRESH   0x00000001U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_1_THRESH1      0x00000002U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_1_THRESH2      0x00000004U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_1_THRESH3      0x00000008U
/* Battery Source 2 Alarm State */
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_2_MIN_THRESH   0x00000400U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_2_THRESH1      0x00000800U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_2_THRESH2      0x00001000U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_2_THRESH3      0x00002000U
/* Battery Source 3 Alarm State */
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_3_MIN_THRESH   0x00100000U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_3_THRESH1      0x00200000U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_3_THRESH2      0x00400000U
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_3_THRESH3      0x00800000U
/* Mains power supply lost */
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_MAIN_LOST      0x40000000U
/* All */
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_MASK           0x40f03c0fU

/* Power Configuration default values */
#define ZCL_POWER_CONFIG_MAINS_ALARM_MASK_DEFAULT       0x00 /* none */
#define ZCL_POWER_CONFIG_MAINS_VOLT_MIN_DEFAULT         0x0000
#define ZCL_POWER_CONFIG_MAINS_VOLT_MAX_DEFAULT         0xFFFF
#define ZCL_POWER_CONFIG_BATTERY_PCT_DEFAULT            0x00
#define ZCL_POWER_CONFIG_BATTERY_SIZE_DEFAULT           0xFF
#define ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_DEFAULT     0x00
#define ZCL_POWER_CONFIG_BATTERY_VOLT_MIN_DEFAULT       0x0000
#define ZCL_POWER_CONFIG_BATTERY_PCT_MIN_DEFAULT        0x00
#define ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_DEFAULT    0x00000000
#define ZCL_POWER_CONFIG_BATTERY2_SIZE_DEFAULT          0xFF
#define ZCL_POWER_CONFIG_BATTERY3_SIZE_DEFAULT          0xFF

#endif /* INC_ZCL_CLUSTER_POWER_CONFIG_HPP_ */
