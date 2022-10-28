#ifndef INC_ZCL_CLUSTER_TEMPERATURE_HPP_
#define INC_ZCL_CLUSTER_TEMPERATURE_HPP_

/*TODO Temperature Measurement*/
/** Temperature Measurement Server Attribute IDs */
enum ZbZclTempMeasSvrAttrT {
    ZCL_TEMP_MEAS_ATTR_MEAS_VAL = 0x0000, /**< MeasuredValue */
    ZCL_TEMP_MEAS_ATTR_MIN_MEAS_VAL = 0x0001, /**< MinMeasuredValue */
    ZCL_TEMP_MEAS_ATTR_MAX_MEAS_VAL = 0x0002, /**< MaxMeasuredValue */
    ZCL_TEMP_MEAS_ATTR_TOLERANCE = 0x0003 /**< Tolerance (Optional) */
};

/* Temperature Measurement Defines */
#define ZCL_TEMP_MEAS_MEASURED_DEFAULT                       0xffff
#define ZCL_TEMP_MEAS_UNKNOWN                                0x8000
#define ZCL_TEMP_MEAS_MIN_MEAS_VAL_MIN              (int16_t)0x954d
#define ZCL_TEMP_MEAS_MIN_MEAS_VAL_MAX              (int16_t)0x7ffe
#define ZCL_TEMP_MEAS_MAX_MEAS_VAL_MIN              (int16_t)0x954e
#define ZCL_TEMP_MEAS_MAX_MEAS_VAL_MAX              (int16_t)0x7fff
#define ZCL_TEMP_MEAS_TOLERANCE_MIN                          0x0000
#define ZCL_TEMP_MEAS_TOLERANCE_MAX                          0x0800

#endif /* INC_ZCL_CLUSTER_TEMPERATURE_HPP_ */
