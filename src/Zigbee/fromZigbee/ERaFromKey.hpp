#ifndef INC_ERA_FROM_KEY_ZIGBEE_HPP_
#define INC_ERA_FROM_KEY_ZIGBEE_HPP_

#define KEY_POWER_CONFIG_BATTERY_VOLTAGE        "battery_voltage"
#define KEY_POWER_CONFIG_BATTERY_PCT            "battery_percent"
#define KEY_POWER_CONFIG_BATTERY_SIZE           "battery_size"
#define KEY_POWER_CONFIG_BATTERY_ALARM_MASK     "battery_alarm_mask"
#define KEY_POWER_CONFIG_BATTERY_VOLT_MIN       "battery_volt_min"
#define KEY_POWER_CONFIG_BATTERY_VTHRESHOLD1    "battery_volt_threshold_1"
#define KEY_POWER_CONFIG_BATTERY_VTHRESHOLD2    "battery_volt_threshold_2"
#define KEY_POWER_CONFIG_BATTERY_VTHRESHOLD3    "battery_volt_threshold_3"
#define KEY_POWER_CONFIG_BATTERY_PCT_MIN        "battery_percent_min"
#define KEY_POWER_CONFIG_BATTERY_PTHRESHOLD1    "battery_percent_threshold_1"
#define KEY_POWER_CONFIG_BATTERY_PTHRESHOLD2    "battery_percent_threshold_2"
#define KEY_POWER_CONFIG_BATTERY_PTHRESHOLD3    "battery_percent_threshold_3"
#define KEY_POWER_CONFIG_BATTERY_ALARM_STATE    "battery_state"

#define KEY_ONOFF_ONOFF                         "state"
#define KEY_ONOFF_ON_TIME                       "on_time"
#define KEY_ONOFF_OFF_WAIT_TIME                 "off_wait_time"
#define KEY_ONOFF_STARTUP_ONOFF                 "startup_onoff"
#define KEY_ONOFF_TUYA_BLACKLIGHT_SWITCH        "tuya_blacklight_switch"
#define KEY_ONOFF_TUYA_BLACKLIGHT_MODE          "tuya_blacklight_mode"
#define KEY_ONOFF_MOES_STARTUP_ONOFF            "moes_startup_onoff"
#define KEY_ONOFF_TUYA_OPERATION_MODE           "tuya_operation_mode"

#define KEY_SPEC_ONOFF_STATE_COMMAND            "state_command"
#define KEY_SPEC_ONOFF_TUYA_BUTTON              "tuya_button"

#define KEY_LEVEL_CURRLEVEL                     "level"
#define KEY_LEVEL_REMAINTIME                    "remaining_time"
#define KEY_LEVEL_MINLEVEL                      "min_level"
#define KEY_LEVEL_MAXLEVEL                      "max_level"
#define KEY_LEVEL_ONOFF_TRANS_TIME              "onoff_transition_time"
#define KEY_LEVEL_ONLEVEL                       "on_level"
#define KEY_LEVEL_ON_TRANS_TIME                 "on_transition_time"
#define KEY_LEVEL_OFF_TRANS_TIME                "off_transition_time"
#define KEY_LEVEL_DEFAULT_MOVE_RATE             "default_move_rate"
#define KEY_LEVEL_STARTUP_CURRLEVEL             "startup_level"

#define KEY_SPEC_LEVEL_LEVEL_COMMAND            "level_command"
#define KEY_SPEC_LEVEL_MOVELEVEL                "level_move_level"
#define KEY_SPEC_LEVEL_MOVELEVEL_ONOFF          "level_move_level_onoff"

#define KEY_MULTISTATE_INPUT_PREVALUE           "in_present_value"

#define KEY_MEAS_TEMPERATURE_MEAS_VAL           "temperature_measured"
#define KEY_MEAS_TEMPERATURE_MIN_MEAS_VAL       "temperature_min_measured"
#define KEY_MEAS_TEMPERATURE_MAX_MEAS_VAL       "temperature_max_measured"
#define KEY_MEAS_TEMPERATURE_TOLERANCE          "temperature_tolerance"

#define KEY_MEAS_PRESSURE_MEAS_VAL              "pressure_measured"
#define KEY_MEAS_PRESSURE_MIN_MEAS_VAL          "pressure_min_measured"
#define KEY_MEAS_PRESSURE_MAX_MEAS_VAL          "pressure_max_measured"
#define KEY_MEAS_PRESSURE_TOLERANCE             "pressure_tolerance"

#define KEY_MEAS_HUMIDITY_MEAS_VAL              "humidity_measured"
#define KEY_MEAS_HUMIDITY_MIN_MEAS_VAL          "humidity_min_measured"
#define KEY_MEAS_HUMIDITY_MAX_MEAS_VAL          "humidity_max_measured"
#define KEY_MEAS_HUMIDITY_TOLERANCE             "humidity_tolerance"

#define KEY_SPEC_GREEN_POWER_COMMAND_ID         "gp_command_id"

#define KEY_IAS_ZONE_ALARM_STATE                "alarm_state"
#define KEY_IAS_ZONE_TAMPER                     "tamper"
#define KEY_IAS_ZONE_BATTERY_LOW                "battery_low"

#define KEY_SPEC_IAS_ZONE_ALARM_STATE           "alarm_state"
#define KEY_SPEC_IAS_ZONE_TAMPER                "tamper"
#define KEY_SPEC_IAS_ZONE_BATTERY_LOW           "battery_low"

#define KEY_CONCAT(zcl, id)                     KEY_ ## zcl ## _ ## id
#define KEY(zcl, id)                            KEY_CONCAT(zcl, id)
#define KEY_SPEC_CONCAT(zcl, id)                KEY_SPEC_ ## zcl ## _ ## id
#define KEY_SPEC(zcl, id)                       KEY_SPEC_CONCAT(zcl, id)

#endif /* INC_ERA_FROM_KEY_ZIGBEE_HPP_ */
