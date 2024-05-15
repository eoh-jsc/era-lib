#ifndef INC_ERA_UUID_HPP_
#define INC_ERA_UUID_HPP_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <Utility/ERaUtility.hpp>

#define ERA_UUID_SEED_MAX 0xFFFFFFFFU

class ERaUUID
{
    enum class Mode
        : uint8_t {
        UUID_MODE_VARIANT4 = 0x00,
        UUID_MODE_RANDOM = 0x01
    };

public:
    ERaUUID()
    {
        this->seed(27, 6);
        this->setVariant4Mode();
        this->generate();
    }

    void seed(uint32_t s1, uint32_t s2 = 0) {
        if (!s1) {
            s1 = 27;
        }
        if (!s2) {
            s2 = 6;
        }
        this->mW = s1;
        this->mZ = s2;
    }

    void generate() {
        uint32_t arr[4] {0};
        for (size_t i = 0; i < 4; ++i) {
            arr[i] = this->random();
        }
        /*
         *  Conforming to RFC 4122 Specification
         *  - byte 7: four most significant bits ==> 0100  --> always 4
         *  - byte 9: two  most significant bits ==> 10    --> always {8, 9, A, B}
         *
         *  patch bits for version 1 and variant 4 here
        */
        if (this->mMode == Mode::UUID_MODE_VARIANT4) {
            arr[1] &= 0xFFF0FFFF;   /* remove 4 bits */
            arr[1] |= 0x00040000;   /* variant 4 */
            arr[2] &= 0xFFFFFFF3;   /* remove 2 bits */
            arr[2] |= 0x00000008;   /* version 1 */
        }

        /* Process 16 bytes build up the char array. */
        for (size_t i = 0, j = 0; i < 16; ++i) {
            /* Multiples of 4 between 8 and 20 get a -. */
            /* Note we are processing 2 digits in one loop. */
            if ((i & 0x1) == 0) {
                if ((4 <= i) && (i <= 10)) {
                    this->mBuffer[j++] = '-';
                }
            }

            /* Process one byte at the time instead of a nibble */
            uint8_t nr = (i / 4);
            uint8_t xx = arr[nr];
            uint8_t ch = (xx & 0x0F);
            this->mBuffer[j++] = ((ch < 10) ? ('0' + ch) : (('a' - 10) + ch));

            ch = ((xx >> 4) & 0x0F);
            arr[nr] >>= 8;
            this->mBuffer[j++] = ((ch < 10) ? ('0' + ch) : (('a' - 10) + ch));
        }

        this->mBuffer[36] = 0;
    }

    char* toCharArray() {
        return this->mBuffer;
    }

    void setVariant4Mode() {
        this->mMode = Mode::UUID_MODE_VARIANT4;
    }

    void setRandomMode() {
        this->mMode = Mode::UUID_MODE_RANDOM;
    }

    uint8_t getMode() {
        return static_cast<uint8_t>(this->mMode);
    }

    static const char* make() {
        static ERaUUID uuid;
        uuid.seed(ERaRandomNumber(0, ERA_UUID_SEED_MAX),
                  ERaFreeRam() + ERaMillis());
        uuid.generate();
        return uuid.toCharArray();
    }

private:
    /*
     * An example of a simple pseudo-random number generator is the
     * Multiply-with-carry method invented by George Marsaglia.
     * two initializers (not null)
    */
    uint32_t random() {
        this->mZ = ((36969L * (this->mZ & 65535L)) + (this->mZ >> 16));
        this->mW = ((18000L * (this->mW & 65535L)) + (this->mW >> 16));
        return ((this->mZ << 16) + this->mW);  /* 32-bit result */
    }

    uint32_t mW {27};
    uint32_t mZ {6};

    char mBuffer[37] {0};
    Mode mMode {Mode::UUID_MODE_VARIANT4};

};

using ERaMessageID = ERaUUID;

#endif /* INC_ERA_UUID_HPP_ */
