#ifndef INC_ERA_DEFINE_MODBUS_HPP_
#define INC_ERA_DEFINE_MODBUS_HPP_

#if !defined(BUILD_UINT16_BE)
    #define BUILD_UINT16_BE(fiByte)     ( (uint16_t)((((fiByte)        << 8 ) & 0xFF00UL) |       \
                                                    (((*(&fiByte + 1))      ) & 0x00FFUL)) )
#endif

#if !defined(BUILD_UINT16_LE)
    #define BUILD_UINT16_LE(fiByte)     ( (uint16_t)((((fiByte)             ) & 0x00FFUL) |       \
                                                    (((*(&fiByte + 1)) << 8 ) & 0xFF00UL)) )
#endif

#if !defined(SWAP_UINT32_BE)
    #define SWAP_UINT32_BE(value)       ( (uint32_t)(((value) << 24) & 0xFF000000UL) | \
                                                    (((value) <<  8) & 0x00FF0000UL) | \
                                                    (((value) >>  8) & 0x0000FF00UL) | \
                                                    (((value) >> 24) & 0x000000FFUL) )
#endif

#define BUILD_BIG_ENDIAN(fiByte)        ( (uint32_t)((((fiByte)        << 24) & 0xFF000000UL) |   \
                                                    (((*(&fiByte + 1)) << 16) & 0x00FF0000UL) |   \
                                                    (((*(&fiByte + 2)) << 8 ) & 0x0000FF00UL) |   \
                                                    (((*(&fiByte + 3))      ) & 0x000000FFUL)) )
#define BUILD_LITTLE_ENDIAN(fiByte)     ( (uint32_t)(*reinterpret_cast<uint32_t*>(&fiByte)) )
#define BUILD_MID_BIG_ENDIAN(fiByte)    ( (uint32_t)((((fiByte) << 16       ) & 0x00FF0000UL) |   \
                                                    (((*(&fiByte + 1)) << 24) & 0xFF000000UL) |   \
                                                    (((*(&fiByte + 2))      ) & 0x000000FFUL) |   \
                                                    (((*(&fiByte + 3)) << 8 ) & 0x0000FF00UL)) )
#define BUILD_MID_LITTLE_ENDIAN(fiByte) ( (uint32_t)((((fiByte) << 8        ) & 0x0000FF00UL) |   \
                                                    (((*(&fiByte + 1))      ) & 0x000000FFUL) |   \
                                                    (((*(&fiByte + 2)) << 24) & 0xFF000000UL) |   \
                                                    (((*(&fiByte + 3)) << 16) & 0x00FF0000UL)) )

#define BUILD_FLOAT(value)              ((float)(*reinterpret_cast<float*>(&value)))

#define BUILD_WORD(hi, lo)              ((uint16_t)((hi << 8) | lo))
#define HI_WORD(a)                      (((a) >> 8) & 0xFF)
#define LO_WORD(a)                      ((a) & 0xFF)

#define MODBUS_SINGLE_COIL_ON           (uint16_t)0xFF00
#define MODBUS_SINGLE_COIL_OFF          (uint16_t)0x0000

#define MODBUS_STRING_SCAN              "scan"
#define MODBUS_STRING_FAIL_READ         "fail_read"
#define MODBUS_STRING_FAIL_WRITE        "fail_write"
#define MODBUS_STRING_TOTAL_READ        "total_read"
#define MODBUS_STRING_TOTAL_WRITE       "total_write"

#if !defined(ERA_MAX_REGISTERS)
    #define ERA_MAX_REGISTERS           ERA_MAX_VIRTUAL_PINS
#endif

#define LOC_BUFFER_MODBUS(len)                      \
    uint8_t locData[32] {0};                        \
    uint8_t* pData = locData;                       \
    uint16_t pDataLen = request->getLength() * len; \
    if (pDataLen > sizeof(locData)) {               \
        pData = (uint8_t*)malloc(pDataLen);         \
        if (pData == nullptr) {                     \
            ERA_ASSERT(pData != nullptr);           \
            return;                                 \
        }                                           \
        memset(pData, 0, pDataLen);                 \
    }

#define FREE_BUFFER_MODBUS                          \
    if (pData != locData) {                         \
        free(pData);                                \
    }                                               \
    pData = nullptr;

enum ModbusFunctionT
    : uint8_t {
    READ_COIL_STATUS = 0x01,
    READ_INPUT_STATUS = 0x02,
    READ_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    FORCE_SINGLE_COIL = 0x05,
    PRESET_SINGLE_REGISTER = 0x06,
    FORCE_MULTIPLE_COILS = 0x0F,
    PRESET_MULTIPLE_REGISTERS = 0x10
};

enum ModbusStatusT
    : uint8_t {
    MODBUS_STATUS_OK = 0x00,
    MODBUS_STATUS_ILLEGAL_FUNCTION = 0x01,
    MODBUS_STATUS_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_STATUS_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_STATUS_SLAVE_DEVICE_FAILURE = 0x04,
    MODBUS_STATUS_ACKNOWLEDGE = 0x05,
    MODBUS_STATUS_SLAVE_DEVICE_BUSY = 0x06,
    MODBUS_STATUS_NEGATIVE_ACKNOWLEDGE = 0x07,
    MODBUS_STATUS_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_STATUS_GATEWAY_PATH_UNAVAILABLE = 0x10,
    MODBUS_STATUS_GATEWAY_FAILED_TO_RESPOND = 0x11,
    MODBUS_STATUS_MAX = 0xFF
};

enum ModbusTransportT
    : uint8_t {
    MODBUS_TRANSPORT_RTU = 0x00,
    MODBUS_TRANSPORT_TCP = 0x01
};

enum ModbusActionTypeT
    : uint8_t {
    MODBUS_ACTION_DEFAULT = 0x00,
    MODBUS_ACTION_PARAMS = 0x01
};

enum ModbusDataT
    : uint8_t {
    MODBUS_BIT = 0x00,
    MODBUS_INT16 = 0x01,
    MODBUS_UINT16 = 0x02,
    MODBUS_INT32 = 0x03,
    MODBUS_UINT32 = 0x04,
    MODBUS_FLOAT = 0x05,

    /* Custom */
    MODBUS_INT_4_NIBBLES_FIRST = 0x06,
    MODBUS_INT_4_NIBBLES_LAST = 0x07
};

enum ModbusConvertT
    : uint8_t {
    MODBUS_BIG_ENDIAN = 0x00,
    MODBUS_LITTLE_ENDIAN = 0x01,
    MODBUS_MID_BIG_ENDIAN = 0x02,
    MODBUS_MID_LITTLE_ENDIAN = 0x03
};

typedef struct __ModbusAction_t {
    char* key;
    uint8_t type;
    uint16_t param;
} ModbusAction_t;

typedef struct __ModbusActionRaw_t {
    uint8_t addr;
    uint8_t func;
    uint8_t sa1;
    uint8_t sa2;
    uint8_t len1;
    uint8_t len2;
    uint8_t* pExtra;
    size_t pExtraLen;
    uint16_t prevdelay;
    uint16_t postdelay;
    IPSlave_t ip;
} ModbusActionRaw_t;

#endif /* INC_ERA_DEFINE_MODBUS_HPP_ */
