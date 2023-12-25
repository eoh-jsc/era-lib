#ifndef INC_DEFINE_SYSTEM_ZIGBEE_HPP_
#define INC_DEFINE_SYSTEM_ZIGBEE_HPP_

namespace ZigbeeNamespace {

    enum SYSCommandsT {
        SYS_RESET_REQ = 0x00,
        SYS_PING,
        SYS_VERSION,
        SYS_SET_EXTADDR,
        SYS_GET_EXTADDR,
        SYS_RAM_READ,
        SYS_RAM_WRITE,
        SYS_OSAL_NV_ITEM_INIT,
        SYS_OSAL_NV_READ,
        SYS_OSAL_NV_WRITE,
        SYS_OSAL_START_TIMER,
        SYS_OSAL_STOP_TIMER,
        SYS_RANDOM,
        SYS_ADC_READ,
        SYS_GPIO,
        SYS_STACK_TUNE,
        SYS_SET_TIME,
        SYS_GET_TIME,
        SYS_OSAL_NV_DELETE,
        SYS_OSAL_NV_LENGTH,
        SYS_SET_TX_POWER, //0x14
        SYS_ZDIAGS_INIT_STATS = 0x17,
        SYS_ZDIAGS_CLEAR_STATS,
        SYS_ZDIAGS_GET_STATS,
        SYS_ZDIAGS_RESTORE_STATS_NV,
        SYS_ZDIAGS_SAVE_STATS_TO_NV,
        SYS_OSAL_NV_READ_EXT,
        SYS_OSAL_NV_WRITE_EXT,
        SYS_NV_CREATE = 0x30,
        SYS_NV_DELETE,
        SYS_NV_LENGTH,
        SYS_NV_READ,
        SYS_NV_WRITE,
        SYS_NV_UPDATE,
        SYS_NV_COMPACT,
        SYS_RESET_IND = 0x80,
        SYS_OSAL_TIMER_EXPIRED
    };

    enum SYSOSALCommandsT {
        SYS_OSAL_NV_READ_EXT_CMD = 0x08,
        SYS_OSAL_NV_WRITE_EXT_CMD
    };

};

#endif /* INC_DEFINE_SYSTEM_ZIGBEE_HPP_ */
