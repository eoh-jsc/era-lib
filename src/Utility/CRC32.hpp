#ifndef INC_CRC32_HPP_
#define INC_CRC32_HPP_

#include <stdint.h>
#include <stddef.h>

class CRC32
{
public:
    CRC32();

    void begin();
    void reset();
    void update(const uint8_t& data);

    template <typename T>
    void update(const T& data) {
        this->update(&data, 1);
    }

    template <typename T>
    void update(const T* data, size_t size) {
        size_t nBytes = size * sizeof(T);
        const uint8_t* pData = (const uint8_t*)data;

        for (size_t i = 0; i < nBytes; ++i) {
            this->update(pData[i]);
        }
    }

    size_t update(uint32_t start, uint32_t end, uint8_t byte) {
        if (end <= start) {
            return 0;
        }
        size_t nBytes = end - start;

        for (size_t i = 0; i < nBytes; ++i) {
            this->update(byte);
        }
        return nBytes;
    }

    uint32_t finalize() const;

    template <typename T>
    static uint32_t calculate(const T* data, size_t size) {
        CRC32 crc;
        crc.update(data, size);
        return crc.finalize();
    }

private:
    uint32_t _state = ~0L;
};

#endif /* INC_CRC32_HPP_ */
