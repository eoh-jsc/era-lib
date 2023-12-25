#ifndef INC_DEFINE_DATATYPE_ZIGBEE_HPP_
#define INC_DEFINE_DATATYPE_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum DataTypeT {
        // TODO: refactor to snake casing
        zcl_noData = 0,
        zcl_data8 = 8,
        zcl_data16 = 9,
        zcl_data24 = 10,
        zcl_data32 = 11,
        zcl_data40 = 12,
        zcl_data48 = 13,
        zcl_data56 = 14,
        zcl_data64 = 15,
        zcl_boolean = 16,
        zcl_bitmap8 = 24,
        zcl_bitmap16 = 25,
        zcl_bitmap24 = 26,
        zcl_bitmap32 = 27,
        zcl_bitmap40 = 28,
        zcl_bitmap48 = 29,
        zcl_bitmap56 = 30,
        zcl_bitmap64 = 31,
        zcl_uint8 = 32,
        zcl_uint16 = 33,
        zcl_uint24 = 34,
        zcl_uint32 = 35,
        zcl_uint40 = 36,
        zcl_uint48 = 37,
        zcl_uint56 = 38,
        zcl_uint64 = 39,
        zcl_int8 = 40,
        zcl_int16 = 41,
        zcl_int24 = 42,
        zcl_int32 = 43,
        zcl_enum8 = 48,
        zcl_enum16 = 49,
        zcl_singlePrec = 57,
        zcl_doublePrec = 58,
        zcl_octetStr = 65,
        zcl_charStr = 66,
        zcl_longOctetStr = 67,
        zcl_longCharStr = 68,
        zcl_array = 72,
        zcl_struct = 76,
        zcl_set = 80,
        zcl_bag = 81,
        zcl_tod = 224,
        zcl_date = 225,
        zcl_utc = 226,
        zcl_clusterId = 232,
        zcl_attrId = 233,
        zcl_bacOid = 234,
        zcl_ieeeAddr = 240,
        zcl_secKey = 241,
        zcl_unknown = 255,

        // TODO: ones below are not in BuffaloZcl and thus cannot be parsed/written
        zcl_int40 = 44,
        zcl_int48 = 45,
        zcl_int56 = 46,
        zcl_int64 = 47,
        zcl_semiPrec = 56
    };

};

#endif /* INC_DEFINE_DATATYPE_ZIGBEE_HPP_ */
