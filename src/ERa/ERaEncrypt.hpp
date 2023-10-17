#ifndef INC_ERA_ENCRYPT_HPP_
#define INC_ERA_ENCRYPT_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>

enum EncryptModeT
    : uint8_t {
    ENCRYPT_MODE_CBC = 0,
    ENCRYPT_MODE_CCM = 1
};

class ERaEncrypt
{
public:
    ERaEncrypt(bool _base64 = false)
        : base64(_base64)
        , mode(EncryptModeT::ENCRYPT_MODE_CBC)
    {}
    ~ERaEncrypt()
    {}

    virtual void begin(const uint8_t* key,
            EncryptModeT modeEnc = EncryptModeT::ENCRYPT_MODE_CBC) = 0;
    virtual void setSecretKey(const uint8_t* key) = 0;
    virtual void setInitializationVector(const uint8_t* nonce,
                        size_t nonceLen) = 0;
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
    EncryptModeT mode;
private:
};

#endif /* INC_ERA_ENCRYPT_HPP_ */
