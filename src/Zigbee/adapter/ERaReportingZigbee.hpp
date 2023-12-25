#ifndef INC_ERA_REPORTING_ZIGBEE_HPP_
#define INC_ERA_REPORTING_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

static const ConfigBindReport_t ReportingList[] = {
    {
        ClusterIDT::ZCL_CLUSTER_POWER_CONFIG, 3, {
            {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE, DataTypeT::zcl_uint8, ReportIntervalT::REP_INTERVAL_HOUR, ReportIntervalT::REP_INTERVAL_MAX, 0x00},
            {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_PCT, DataTypeT::zcl_uint8, ReportIntervalT::REP_INTERVAL_HOUR, ReportIntervalT::REP_INTERVAL_MAX, 0x00},
            {ZbZclPowerConfigSvrAttrT::ZCL_POWER_CONFIG_ATTR_BATTERY_ALARM_STATE, DataTypeT::zcl_bitmap32, ReportIntervalT::REP_INTERVAL_HOUR, ReportIntervalT::REP_INTERVAL_MAX, 0x00000000}
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_ONOFF, 1, {
            {ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_ONOFF, DataTypeT::zcl_boolean, ReportIntervalT::REP_INTERVAL_NONE, ReportIntervalT::REP_INTERVAL_HOUR, 0x00},
        }, ManufacturerCodesT::MANUF_CODE_NONE, 1, {
            ZbZclOnOffSvrAttrT::ZCL_ONOFF_ATTR_ONOFF
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, 1, {
            {ZbZclLevelSvrAttrT::ZCL_LEVEL_ATTR_CURRLEVEL, DataTypeT::zcl_uint8, ReportIntervalT::REP_INTERVAL_NONE, ReportIntervalT::REP_INTERVAL_HOUR, 0x01},
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE, 1, {
            {ZbZclTempMeasSvrAttrT::ZCL_TEMP_MEAS_ATTR_MEAS_VAL, DataTypeT::zcl_int16, ReportIntervalT::REP_INTERVAL_MINUTE, ReportIntervalT::REP_INTERVAL_HOUR, 0x001B},
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE, 1, {
            {ZbZclPressMeasSvrAttrT::ZCL_PRESS_MEAS_ATTR_MEAS_VAL, DataTypeT::zcl_int16, ReportIntervalT::REP_INTERVAL_MINUTE, ReportIntervalT::REP_INTERVAL_HOUR, 0x0005},
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY, 1, {
            {ZbZclHumiMeasSvrAttrT::ZCL_HUMI_MEAS_ATTR_MEAS_VAL, DataTypeT::zcl_uint16, ReportIntervalT::REP_INTERVAL_MINUTE, ReportIntervalT::REP_INTERVAL_HOUR, 0x0064},
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE
    }
};

#endif /* INC_ERA_REPORTING_ZIGBEE_HPP_ */
