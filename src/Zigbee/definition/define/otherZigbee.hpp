#ifndef INC_DEFINE_OTHER_ZIGBEE_HPP_
#define INC_DEFINE_OTHER_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum ReportIntervalT {
        REP_INTERVAL_NONE = 0,
        REP_INTERVAL_SECOND_1 = 1,
        REP_INTERVAL_SECOND_30 = 30,
        REP_INTERVAL_MINUTE = 60,
        REP_INTERVAL_MINUTE_2 = 120,
        REP_INTERVAL_MINUTES_5 = 300,
        REP_INTERVAL_MINUTES_10 = 600,
        REP_INTERVAL_MINUTES_15 = 900,
        REP_INTERVAL_MINUTES_30 = 1800,
        REP_INTERVAL_HOUR = 3600,
        REP_INTERVAL_MAX = 62000,
    };

    enum FormatDataT {
        FORMAT_DATA_NUMBER = 0,
        FORMAT_DATA_STRING = 1
    };

    enum LeaveIndRemoveT {
        LEAVE_IND_REMOVE_NONE = 0,
        LEAVE_IND_REMOVE_CHILDREN = 1
    };

    enum LeaveIndRejoinT {
        LEAVE_IND_REJOIN_NONE = 0,
        LEAVE_IND_REJOIN = 1
    };

    enum OperationModeT {
        SET_TRANSMIT_POWER = 0,
        SET_RX_ON_WHEN_IDLE = 1
    };

    enum ZigbeeActionT {
        ZIGBEE_ACTION_SET = 0x00,
        ZIGBEE_ACTION_GET = 0x01,
        ZIGBEE_ACTION_GROUP = 0x02,
        ZIGBEE_ACTION_PERMIT_JOIN = 0x03,
        ZIGBEE_ACTION_REMOVE_DEVICE = 0x04
    };

};

#endif /* INC_DEFINE_OTHER_ZIGBEE_HPP_ */
