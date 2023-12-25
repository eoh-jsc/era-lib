#ifndef INC_DEFINE_FOUNDATION_ZIGBEE_HPP_
#define INC_DEFINE_FOUNDATION_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum CommandIdT {
        READ_ATTR = 0x00,            /*attrId - u16, response 1*/
        READ_ATTR_RSP,                /*attrId - u16, status u8, dataType u8 - statusEquals - Zcl_Status SUCCESS, attrData*/
        WRITE_ATTR,                    /*attrId - u16, dataType u8, attrData, response 4*/
        WRITE_ATTR_UNDIV,            /*attrId - u16, dataType u8, attrData*/
        WRITE_ATTR_RSP,                /*status u8, attrId - u16 - statusNotEquals - Zcl_Status SUCCESS*/
        WRITE_ATTR_NO_RSP,            /*attrId - u16, dataType u8, attrData*/
        CONFIG_REPORT,                /*direction - u8, attrId - u16, dataType u8 - directionEquals - CLIENT_TO_SERVER, minRepIntval u16 - CLIENT_TO_SERVER, maxRepIntval u16 - CLIENT_TO_SERVER, repChange - CLIENT_TO_SERVER - ANALOG, timeout u16 - SERVER_TO_CLIENT, response 7*/
        CONFIG_REPORT_RSP,            /*status - u8, direction - u8 (minimumRemainingBufferBytes) - value: 3, attrId - u16 (minimumRemainingBufferBytes) - value: 2*/
        READ_REPORT_CONFIG,            /*direction - u8, attrId - u16*/
        READ_REPORT_CONFIG_RSP,        /*direction - u8, attrId - u16, dataType u8 - directionEquals - CLIENT_TO_SERVER, minRepIntval u16 - CLIENT_TO_SERVER, maxRepIntval u16 - CLIENT_TO_SERVER, repChange - CLIENT_TO_SERVER - ANALOG, timeout u16 - SERVER_TO_CLIENT*/
        REPORT_ATTR,                /*attrId - u16, dataType u8, attrData*/
        DEFAULT_RSP,                /*cmdId - u8, statusCode - u8*/
        DISCO_ATTR,                    /*startAttrId - u16, maxAttrIds - u8*/
        DISCO_ATTR_RSP,                /*attrId - u16, dataType - u8*/
        READ_ATTR_STRUCT,
        WRITE_ATTR_STRUCT,
        WRITE_ATTR_STRUCT_RSP,
        DISCO_CMD_RECEIVE,
        DISCO_CMD_RECEIVE_RSP,
        DISCO_CMD_GENERATE,
        DISCO_CMD_GENERATE_RSP,
        DISCO_ATTR_EXTEND,
        DISCO_ATTR_EXTEND_RSP
    };

};

#endif /* INC_DEFINE_FOUNDATION_ZIGBEE_HPP_ */
