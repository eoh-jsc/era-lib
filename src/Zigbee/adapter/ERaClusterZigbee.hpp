#ifndef INC_ERA_CLUSTER_ZIGBEE_HPP_
#define INC_ERA_CLUSTER_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

static const ClusterIDT AllZclId[] = {
    /* General cluster identifiers. */
    ClusterIDT::ZCL_CLUSTER_BASIC,
    ClusterIDT::ZCL_CLUSTER_POWER_CONFIG,
    ClusterIDT::ZCL_CLUSTER_DEVICE_TEMPERATURE,
    ClusterIDT::ZCL_CLUSTER_IDENTIFY,
    ClusterIDT::ZCL_CLUSTER_GROUPS,
    ClusterIDT::ZCL_CLUSTER_SCENES,
    ClusterIDT::ZCL_CLUSTER_ONOFF,
    ClusterIDT::ZCL_CLUSTER_ONOFF_CONFIG,
    ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL,
    ClusterIDT::ZCL_CLUSTER_ALARMS,
    ClusterIDT::ZCL_CLUSTER_TIME,
    ClusterIDT::ZCL_CLUSTER_RSSI_LOCATION,
    ClusterIDT::ZCL_CLUSTER_ANALOG_INPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_ANALOG_OUTPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_ANALOG_VALUE_BASIC,
    ClusterIDT::ZCL_CLUSTER_BINARY_INPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_BINARY_OUTPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_BINARY_VALUE_BASIC,
    ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_MULTISTATE_OUTPUT_BASIC,
    ClusterIDT::ZCL_CLUSTER_MULTISTATE_VALUE_BASIC,
    ClusterIDT::ZCL_CLUSTER_COMMISSIONING,
    ClusterIDT::ZCL_CLUSTER_PARTITION,
    ClusterIDT::ZCL_CLUSTER_OTA_UPGRADE,
    ClusterIDT::ZCL_CLUSTER_POWER_PROFILE,
    ClusterIDT::ZCL_CLUSTER_APPLIANCE_CONTROL,
    ClusterIDT::ZCL_CLUSTER_POLL_CONTROL,
    ClusterIDT::ZCL_CLUSTER_GREEN_POWER,

    /* Retail clusters. */
    ClusterIDT::ZCL_CLUSTER_MOBILE_DEVICE_CONFIGURATION,
    ClusterIDT::ZCL_CLUSTER_NEIGHBOR_CLEANING,
    ClusterIDT::ZCL_CLUSTER_NEAREST_GATEWAY,
    ClusterIDT::ZCL_CLUSTER_KEEP_ALIVE,
    ClusterIDT::ZCL_CLUSTER_METER_ID,
    ClusterIDT::ZCL_CLUSTER_DIAGNOSTICS,

    /* Closures Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_CLOSURE_SHADE_CONFIG,
    ClusterIDT::ZCL_CLUSTER_DOOR_LOCK,
    ClusterIDT::ZCL_CLUSTER_WINDOW_COVERING,
    ClusterIDT::ZCL_CLUSTER_BARRIER_CONTROL,

    /* HVAC Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_HVAC_PUMP,
    ClusterIDT::ZCL_CLUSTER_HVAC_THERMOSTAT,
    ClusterIDT::ZCL_CLUSTER_HVAC_FAN,
    ClusterIDT::ZCL_CLUSTER_HVAC_DEHUMIDIFIER,
    ClusterIDT::ZCL_CLUSTER_HVAC_THERMOSTAT_UI,

    /* Lighting Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_COLOR_CONTROL,
    ClusterIDT::ZCL_CLUSTER_BALLAST_CONTROL,

    /* Measurement Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_MEAS_ILLUMINANCE,
    ClusterIDT::ZCL_CLUSTER_MEAS_ILLUMINANCE_LEVEL,
    ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE,
    ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE,
    ClusterIDT::ZCL_CLUSTER_MEAS_FLOW,
    ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY,
    ClusterIDT::ZCL_CLUSTER_MEAS_OCCUPANCY,
    ClusterIDT::ZCL_CLUSTER_MEAS_ELECTRICAL,

    /* Security Cluster IDs */
    ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE,
    ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ANCILLARY,
    ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_WARNING,

    /* Smart Energy */
    ClusterIDT::ZCL_CLUSTER_PRICE,
    ClusterIDT::ZCL_CLUSTER_DRLC,
    ClusterIDT::ZCL_CLUSTER_SIMPLE_METERING,
    ClusterIDT::ZCL_CLUSTER_MESSAGING,
    ClusterIDT::ZCL_CLUSTER_TUNNELING,
    ClusterIDT::ZCL_CLUSTER_PREPAYMENT,
    ClusterIDT::ZCL_CLUSTER_ENERGY_MANAGEMENT, /* SE 1.2 */
    ClusterIDT::ZCL_CLUSTER_CALENDAR,
    ClusterIDT::ZCL_CLUSTER_DEVICE_MANAGEMENT, /* SE 1.2 */
    ClusterIDT::ZCL_CLUSTER_EVENTS, /* SE 1.2 */
    ClusterIDT::ZCL_CLUSTER_MDU_PAIRING, /* SE 1.2 */
    ClusterIDT::ZCL_CLUSTER_SUB_GHZ, /* SE 1.2 */

    /* TOUCHLINK */
    ClusterIDT::ZCL_CLUSTER_TOUCHLINK,

    /* Custom, internal use only, for handling cluster persistence.
     * Used with ZCL_MANUF_CODE_INTERNAL. */
    ClusterIDT::ZCL_CLUSTER_PERSIST,

    /* Custom, internal use only, for local loop-back commands we want to
     * send to all clusters. Used with ZCL_MANUF_CODE_INTERNAL. */
    ClusterIDT::ZCL_CLUSTER_WILDCARD,

    /* NO CLUSTER */
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
        ClusterIDT::ZCL_CLUSTER_DEVICE_TEMPERATURE, "genDeviceTempConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_IDENTIFY, "genIdentify"
    },
    {
        ClusterIDT::ZCL_CLUSTER_GROUPS, "genGroups"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SCENES, "genScenes"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ONOFF, "genOnOff"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ONOFF_CONFIG, "genOnOffSwitchConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_LEVEL_CONTROL, "genLevelControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ALARMS, "genAlarms"
    },
    {
        ClusterIDT::ZCL_CLUSTER_TIME, "genTime"
    },
    {
        ClusterIDT::ZCL_CLUSTER_RSSI_LOCATION, "genRssiLocation"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ANALOG_INPUT_BASIC, "genAnalogInput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ANALOG_OUTPUT_BASIC, "genAnalogOutput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ANALOG_VALUE_BASIC, "genAnalogValue"
    },
    {
        ClusterIDT::ZCL_CLUSTER_BINARY_INPUT_BASIC, "genBinaryInput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_BINARY_OUTPUT_BASIC, "genBinaryOutput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_BINARY_VALUE_BASIC, "genBinaryValue"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_INPUT_BASIC, "genMultistateInput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_OUTPUT_BASIC, "genMultistateOutput"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MULTISTATE_VALUE_BASIC, "genMultistateValue"
    },
    {
        ClusterIDT::ZCL_CLUSTER_COMMISSIONING, "genCommissioning"
    },
    {
        ClusterIDT::ZCL_CLUSTER_PARTITION, "genPartition"
    },
    {
        ClusterIDT::ZCL_CLUSTER_OTA_UPGRADE, "genOta"
    },
    {
        ClusterIDT::ZCL_CLUSTER_POWER_PROFILE, "genPowerProfile"
    },
    {
        ClusterIDT::ZCL_CLUSTER_APPLIANCE_CONTROL, "genApplianceControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_POLL_CONTROL, "genPollControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_GREEN_POWER, "greenPower"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MOBILE_DEVICE_CONFIGURATION, "mobileDeviceConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_NEIGHBOR_CLEANING, "neighborCleaning"
    },
    {
        ClusterIDT::ZCL_CLUSTER_NEAREST_GATEWAY, "nearestGateway"
    },
    {
        ClusterIDT::ZCL_CLUSTER_KEEP_ALIVE, "keepAlive"
    },
    {
        ClusterIDT::ZCL_CLUSTER_METER_ID, "haMeterIdentification"
    },
    {
        ClusterIDT::ZCL_CLUSTER_DIAGNOSTICS, "haDiagnostic"
    },
    {
        ClusterIDT::ZCL_CLUSTER_CLOSURE_SHADE_CONFIG, "closuresShadeConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_DOOR_LOCK, "closuresDoorLock"
    },
    {
        ClusterIDT::ZCL_CLUSTER_WINDOW_COVERING, "closuresWindowCovering"
    },
    {
        ClusterIDT::ZCL_CLUSTER_BARRIER_CONTROL, "barrierControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_HVAC_PUMP, "hvacPumpConfigControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_HVAC_THERMOSTAT, "hvacThermostat"
    },
    {
        ClusterIDT::ZCL_CLUSTER_HVAC_FAN, "hvacFanControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_HVAC_DEHUMIDIFIER, "hvacDehumidificationControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_HVAC_THERMOSTAT_UI, "hvacThermostatUi"
    },
    {
        ClusterIDT::ZCL_CLUSTER_COLOR_CONTROL, "lightingColorControl"
    },
    {
        ClusterIDT::ZCL_CLUSTER_BALLAST_CONTROL, "lightingBallastConfig"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_ILLUMINANCE, "msIlluminanceMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_ILLUMINANCE_LEVEL, "msIlluminanceLevelSensing"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_TEMPERATURE, "msTemperatureMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_PRESSURE, "msPressureMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_FLOW, "msFlowMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_HUMIDITY, "msRelativeHumidity"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_OCCUPANCY, "msOccupancySensing"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MEAS_ELECTRICAL, "haElectricalMeasurement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ZONE, "ssIasZone"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_ANCILLARY, "ssIasAce"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SECURITY_IAS_WARNING, "ssIasWd"
    },
    {
        ClusterIDT::ZCL_CLUSTER_PRICE, "sePrice"
    },
    {
        ClusterIDT::ZCL_CLUSTER_DRLC, "seDRLC"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SIMPLE_METERING, "seMetering"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MESSAGING, "seMessaging"
    },
    {
        ClusterIDT::ZCL_CLUSTER_TUNNELING, "seTunneling"
    },
    {
        ClusterIDT::ZCL_CLUSTER_PREPAYMENT, "sePrepayment"
    },
    {
        ClusterIDT::ZCL_CLUSTER_ENERGY_MANAGEMENT, "seEnergyManagement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_CALENDAR, "seCalendar"
    },
    {
        ClusterIDT::ZCL_CLUSTER_DEVICE_MANAGEMENT, "seDeviceManagement"
    },
    {
        ClusterIDT::ZCL_CLUSTER_EVENTS, "seEvents"
    },
    {
        ClusterIDT::ZCL_CLUSTER_MDU_PAIRING, "seMduPairing"
    },
    {
        ClusterIDT::ZCL_CLUSTER_SUB_GHZ, "seSubGHz "
    },
    {
        ClusterIDT::ZCL_CLUSTER_TOUCHLINK, "touchlink"
    },
    {
        ClusterIDT::ZCL_CLUSTER_PERSIST, "persist"
    },
    {
        ClusterIDT::ZCL_CLUSTER_WILDCARD, "wildcard"
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
