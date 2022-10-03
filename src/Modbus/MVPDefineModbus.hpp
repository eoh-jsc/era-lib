#ifndef INC_MVP_DEFINE_MODBUS_HPP_
#define INC_MVP_DEFINE_MODBUS_HPP_

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

#endif /* INC_MVP_DEFINE_MODBUS_HPP_ */