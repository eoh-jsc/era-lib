#ifndef INC_ERA_CLUSTER_ZIGBEE_HPP_
#define INC_ERA_CLUSTER_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

static const ClusterIDT AllZclId[] = {
    ClusterIDT::ZCL_CLUSTER_BASIC,
    ClusterIDT::ZCL_CLUSTER_POWER_CONFIG,
    ClusterIDT::ZCL_CLUSTER_ONOFF,
    ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL,
    ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_OTA_UPGRADE,
    ClusterIDT::ZCL_CLUSTER_POLL_CONTROL,
    ClusterIDT::ZCL_CLUSTER_GREEN_POWER,

    /* Measurement Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE,
    ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE,
    ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY,

    /* Security Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE,

    ClusterIDT::NO_CLUSTER_ID
};

static const StringifyZcl_t StrifyZcl[] = {
    {
        ClusterIDT::ZCL_CLUSTER_BASIC, "genBasic"
    },
    {
        ClusterIDT::ZCL_CLUSTER_POWER_CONFIG, "genPowerConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ONOFF, "genOnOff"
    },
    {
        ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, "genLevelControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC, "genMultistateInput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_OTA_UPGRADE, "genOta"
    },
    {
        ClusterIDT::ZCL_CLUSTER_POLL_CONTROL, "genPollControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_GREEN_POWER, "greenPower"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE, "msTemperatureMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE, "msPressureMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY, "msRelativeHumidity"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE, "ssIasZone"
    },
    {
        ClusterIDT::NO_CLUSTER_ID, "noClusterId"
    }
};

static const KeyDataZigbee_t KeyDataZb[] = {
    {
        ClusterIDT::ZCL_CLUSTER_POWER_CONFIG, 2, {
            KEY(POWER_CONFIG, BATTERY_VOLTAGE), KEY(POWER_CONFIG, BATTERY_PCT)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_ONOFF, 1, {
            KEY(ONOFF, ONOFF)
        }, 1, {
            KEY_SPEC(ONOFF, STATE_COMMAND)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, 1, {
            KEY(LEVEL, CURRLEVEL)
        }, 3, {
            KEY_SPEC(LEVEL, LEVEL_COMMAND), KEY_SPEC(LEVEL, MOVELEVEL), KEY_SPEC(LEVEL, MOVELEVEL_ONOFF)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC, 1, {
            KEY(MULTISTATE_INPUT, PREVALUE)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_GREEN_POWER, 0, {}, 1,  {
            KEY_SPEC(GREEN_POWER, COMMAND_ID)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE, 1, {
            KEY(MEAS_TEMPERATURE, MEAS_VAL)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE, 1, {
            KEY(MEAS_PRESSURE, MEAS_VAL)
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY, 1, {
            KEY(MEAS_HUMIDITY, MEAS_VAL) 
        }
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE, 3, {
            KEY(IAS_ZONE, ALARM_STATE), KEY(IAS_ZONE, TAMPER), KEY(IAS_ZONE, BATTERY_LOW)
        }, 3, {
            KEY_SPEC(IAS_ZONE, ALARM_STATE), KEY_SPEC(IAS_ZONE, TAMPER), KEY_SPEC(IAS_ZONE, BATTERY_LOW)
        }
    },
};

#endif /* INC_ERA_CLUSTER_ZIGBEE_HPP_ */
