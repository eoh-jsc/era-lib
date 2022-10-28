#ifndef INC_ZCL_CLUSTER_HUMIDITY_HPP_
#define INC_ZCL_CLUSTER_HUMIDITY_HPP_

/*TODO Humidity Measurement*/
/** Humidity Measurement Server Attribute IDs */
enum ZbZclHumiMeasSvrAttrT {
    ZCL_HUMI_MEAS_ATTR_MEAS_VAL = 0x0000, /**< MeasuredValue */
    ZCL_HUMI_MEAS_ATTR_MIN_MEAS_VAL = 0x0001, /**< MinMeasuredValue */
    ZCL_HUMI_MEAS_ATTR_MAX_MEAS_VAL = 0x0002, /**< MaxMeasuredValue */
    ZCL_HUMI_MEAS_ATTR_TOLERANCE = 0x0003 /**< Tolerance (Optional) */
};

#endif /* INC_ZCL_CLUSTER_HUMIDITY_HPP_ */
