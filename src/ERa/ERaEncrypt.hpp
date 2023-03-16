#ifndef INC_ERA_ENCRYPT_HPP_
#define INC_ERA_ENCRYPT_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>

class ERaEncrypt
{
public:
    ERaEncrypt(bool _base64 = false)
        : base64(_base64)
    {}
    ~ERaEncrypt()
    {}

    virtual void begin(const uint8_t* key) = 0;
    virtual void setSecretKey(const uint8_t* key) = 0;
    virtual size_t encrypt(const uint8_t* input, size_t inputLen,
                        uint8_t*& output, size_t& outputLen) = 0;
    virtual size_t decrypt(const uint8_t* input, size_t inputLen,
                        uint8_t*& output, size_t& outputLen) = 0;

protected:
    size_t base64EncodeExpectedLen(size_t length) const {
        return ((((4 * length) / 3) + 3) & ~3);
    };

    size_t base64DecodeExpectedLen(size_t length) const {
        return ((length * 3) / 4);
    }

    bool base64;
private:
};

#endif /* INC_ERA_ENCRYPT_HPP_ */
