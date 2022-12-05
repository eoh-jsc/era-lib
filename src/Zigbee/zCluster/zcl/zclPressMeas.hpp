#ifndef INC_ZCL_CLUSTER_PRESSURE_HPP_
#define INC_ZCL_CLUSTER_PRESSURE_HPP_

/*TODO Pressure Measurement*/
/** Pressure Measurement Attribute IDs */
enum ZbZclPressMeasSvrAttrT {
    ZCL_PRESS_MEAS_ATTR_MEAS_VAL = 0x0000, /**< MeasuredValue */
    ZCL_PRESS_MEAS_ATTR_MIN_MEAS_VAL = 0x0001, /**< MinMeasuredValue */
    ZCL_PRESS_MEAS_ATTR_MAX_MEAS_VAL = 0x0002, /**< MaxMeasuredValue */
    ZCL_PRESS_MEAS_ATTR_TOLERANCE = 0x0003, /**< Tolerance (Optional) */
    ZCL_PRESS_MEAS_ATTR_SCALED_VAL = 0x0010, /**< ScaledValue (Optional) */
    ZCL_PRESS_MEAS_ATTR_MIN_SCALED_VAL = 0x0011, /**< MinScaledValue (Optional) */
    ZCL_PRESS_MEAS_ATTR_MAX_SCALED_VAL = 0x0012, /**< MaxScaledValue (Optional) */
    ZCL_PRESS_MEAS_ATTR_SCALED_TOL = 0x0013, /**< ScaledTolerance (Optional) */
    ZCL_PRESS_MEAS_ATTR_SCALE = 0x0014 /**< Scale (Optional) */
};

/* Pressure Measurement Defines */
#define ZCL_PRESS_MEAS_UNKNOWN                       (int16_t)0x8000
#define ZCL_PRESS_MEAS_MEAS_VAL_DEFAULT              (int16_t)0xffff
#define ZCL_PRESS_MEAS_MIN_VAL_MIN                   (int16_t)0x8001
#define ZCL_PRESS_MEAS_MIN_VAL_MAX                   (int16_t)0x7ffe
#define ZCL_PRESS_MEAS_MAX_VAL_MIN                   (int16_t)0x8002
#define ZCL_PRESS_MEAS_MAX_VAL_MAX                   (int16_t)0x7fff
#define ZCL_PRESS_MEAS_SCALE_MIN                     (int8_t)0x81
#define ZCL_PRESS_MEAS_SCALE_MAX                     (int8_t)0x7f

#endif /* INC_ZCL_CLUSTER_PRESSURE_HPP_ */
