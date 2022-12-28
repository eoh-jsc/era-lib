#ifndef INC_ERA_DEFINE_MODBUS_HPP_
#define INC_ERA_DEFINE_MODBUS_HPP_

#define BUILD_UINT16_BE(fiByte)         ((uint16_t)((fiByte) << 8)              \
                                                    | (*(&fiByte + 1)))

#define BUILD_BIG_ENDIAN(fiByte)        ((uint32_t)((fiByte << 24)              \
                                                    | ((*(&fiByte + 1)) << 16)  \
                                                    | ((*(&fiByte + 2)) << 8)   \
                                                    | ((*(&fiByte + 3))))
#define BUILD_LITTE_ENDIAN(fiByte)      ((uint32_t)(*reinterpret_cast<uint32_t*>(&fiByte)))
#define BUILD_MID_BIG_ENDIAN(fiByte)    ((uint32_t)((fiByte << 16)              \
                                                    | ((*(&fiByte + 1)) << 24)  \
                                                    | ((*(&fiByte + 2)))        \
                                                    | ((*(&fiByte + 3)) << 8))
#define BUILD_MID_MID_ENDIAN(fiByte)    ((uint32_t)((fiByte << 8)               \
                                                    | ((*(&fiByte + 1)))        \
                                                    | ((*(&fiByte + 2)) << 24)  \
                                                    | ((*(&fiByte + 3)) << 16))

#define BUILD_FLOAT(value)              ((float)(*reinterpret_cast<float*>(&value)))

#define BUILD_WORD(hi, lo)              ((uint16_t)((hi << 8) | lo))
#define HI_WORD(a)                      (((a) >> 8) & 0xFF)
#define LO_WORD(a)                      ((a) & 0xFF)

enum ModbusFunctionT : uint8_t {
    READ_COIL_STATUS = 0x01,
    READ_INPUT_STATUS = 0x02,
    READ_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    FORCE_SINGLE_COIL = 0x05,
    PRESET_SINGLE_REGISTER = 0x06,
    FORCE_MULTIPLE_COILS = 0x0F,
    PRESET_MULTIPLE_REGISTERS = 0x10
};

enum ModbusTransportT : uint8_t {
    MODBUS_TRANSPORT_RTU = 0x00,
    MODBUS_TRANSPORT_TCP = 0x01
};

#endif /* INC_ERA_DEFINE_MODBUS_HPP_ */
