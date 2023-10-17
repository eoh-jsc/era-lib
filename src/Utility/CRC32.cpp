#include <Utility/CRC32.hpp>

#if defined(__AVR__) || \
    (defined(ARDUINO) && defined(ESP8266))
    #include <avr/pgmspace.h>
#endif

#if defined(PROGMEM)
    #define CRC_PROGMEM PROGMEM
    #define CRC_READ_DWORD(x) (pgm_read_dword_near(x))
#else
    #define CRC_PROGMEM
    #define CRC_READ_DWORD(x) (*(uint32_t*)(x))
#endif

static const uint32_t crc32_table[] CRC_PROGMEM = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

CRC32::CRC32() {
    reset();
}

void CRC32::begin() {
    _state = ~0L;
}

void CRC32::reset() {
    _state = ~0L;
}

void CRC32::update(const uint8_t& data) {
    uint8_t tbl_idx = 0;

    tbl_idx = _state ^ (data >> (0 * 4));
    _state = CRC_READ_DWORD(crc32_table + (tbl_idx & 0x0f)) ^ (_state >> 4);
    tbl_idx = _state ^ (data >> (1 * 4));
    _state = CRC_READ_DWORD(crc32_table + (tbl_idx & 0x0f)) ^ (_state >> 4);
}

uint32_t CRC32::finalize() const {
    return ~_state;
}
