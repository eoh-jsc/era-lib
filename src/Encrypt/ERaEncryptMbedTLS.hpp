#ifndef INC_ERA_ENCRYPT_MBEDTLS_HPP_
#define INC_ERA_ENCRYPT_MBEDTLS_HPP_

#include <string.h>
#include <mbedtls/platform.h>
#include <mbedtls/aes.h>
#include <mbedtls/base64.h>
#include <mbedtls/cipher.h>
#include <ERa/ERaEncrypt.hpp>
#include <ERa/ERaDefine.hpp>
#include <Utility/ERaLoc.hpp>

#define CF_CHECK_EQ(expr, res)              \
    if ((expr) != (res)) {                  \
        mbedtls_cipher_free(&this->ctx);    \
        return 0;                           \
    }

#define CF_CHECK_EQB(expr, res)             \
    if ((expr) != (res)) {                  \
        if (locInput != input) {            \
            free(locInput);                 \
            locInput = nullptr;             \
        }                                   \
        free(output);                       \
        output = nullptr;                   \
        mbedtls_cipher_free(&this->ctx);    \
        return 0;                           \
    }

#define CF_CHECK_NE(expr, res)              \
    if ((expr) == (res)) {                  \
        mbedtls_cipher_free(&this->ctx);    \
        return 0;                           \
    }

#define CF_INPUT_SIZE   256

class ERaEncryptMbedTLS
    : public ERaEncrypt
{
public:
    ERaEncryptMbedTLS(bool _base64 = false)
        : ERaEncrypt(_base64)
    {}
    ~ERaEncryptMbedTLS()
    {}

    void begin(const uint8_t* key) override {
        this->setSecretKey(key);
        mbedtls_platform_set_calloc_free(ERA_CALLOC, ERA_FREE);
    }

    void setSecretKey(const uint8_t* key) override {
        memcpy(this->secretKey, key, sizeof(this->secretKey));
    }

    size_t encrypt(const uint8_t* input, size_t inputLen,
                uint8_t*& output, size_t& outputLen) override {
        return this->progress(input, inputLen, output, outputLen,
                            mbedtls_operation_t::MBEDTLS_ENCRYPT);
    }

    size_t decrypt(const uint8_t* input, size_t inputLen,
                uint8_t*& output, size_t& outputLen) override {
        return this->progress(input, inputLen, output, outputLen,
                            mbedtls_operation_t::MBEDTLS_DECRYPT);
    }

protected:
private:
    size_t progress(const uint8_t* input, size_t inputLen,
                    uint8_t*& output, size_t& outputLen,
                    mbedtls_operation_t operation) {
        uint8_t tag[16] {0};
        size_t oLen {0};
        size_t oSize {0};
        outputLen = 0;
        const mbedtls_cipher_info_t* cipher_info = nullptr;

        mbedtls_cipher_init(&this->ctx);

        CF_CHECK_NE(cipher_info = mbedtls_cipher_info_from_type(mbedtls_cipher_type_t::MBEDTLS_CIPHER_AES_128_CBC), nullptr)

        CF_CHECK_EQ(mbedtls_cipher_setup(&this->ctx, cipher_info), 0)

        CF_CHECK_EQ(mbedtls_cipher_setkey(&this->ctx, this->secretKey, 128, operation), 0)

        CF_CHECK_EQ(mbedtls_cipher_set_iv(&this->ctx, this->iv, sizeof(this->iv)), 0)

        CF_CHECK_EQ(mbedtls_cipher_reset(&this->ctx), 0)

        CF_CHECK_EQ(mbedtls_cipher_update_ad(&this->ctx, nullptr, 0), 0)

        uint8_t* locInput = (uint8_t*)input;
        if (operation == mbedtls_operation_t::MBEDTLS_DECRYPT) {
            if (this->decode(input, inputLen, locInput, inputLen) != 0) {
                if (locInput != input) {
                    free(locInput);
                    locInput = nullptr;
                }
                mbedtls_cipher_free(&this->ctx);
                return 0;
            }
        }

        if (operation == mbedtls_operation_t::MBEDTLS_ENCRYPT) {
            oSize = (inputLen / 16) + 1;
            oSize = oSize * 16;
        }
        else {
            oSize = (inputLen / 16);
            oSize = oSize * 16;
        }

        output = (uint8_t*)ERA_CALLOC(oSize + 1, sizeof(uint8_t));
        if (output == nullptr) {
            if (locInput != input) {
                free(locInput);
                locInput = nullptr;
            }
            mbedtls_cipher_free(&this->ctx);
            return 0;
        }

        size_t alignUp = ((inputLen - 1) / CF_INPUT_SIZE) + 1;
        for (size_t i = 0; i < alignUp; ++i) {
            size_t alignDown = inputLen - (i * CF_INPUT_SIZE);
            if (!(alignDown / CF_INPUT_SIZE)) {
                alignDown %= CF_INPUT_SIZE;
            }
            else {
                alignDown = CF_INPUT_SIZE;
            }
            if ((outputLen + alignDown) > oSize) {
                if (locInput != input) {
                    free(locInput);
                    locInput = nullptr;
                }
                free(output);
                output = nullptr;
                mbedtls_cipher_free(&this->ctx);
                return 0;
            }
            CF_CHECK_EQB(mbedtls_cipher_update(&this->ctx, locInput + (i * CF_INPUT_SIZE),
                                            alignDown, output + outputLen, &oLen), 0)
            outputLen += oLen;
        }

        CF_CHECK_EQB(mbedtls_cipher_finish(&this->ctx, output + outputLen, &oLen), 0)
        outputLen += oLen;
        output[outputLen] = 0;

        uint8_t* locOutput = output;
        if (operation == mbedtls_operation_t::MBEDTLS_ENCRYPT) {
            this->encode(output, outputLen, locOutput, outputLen);
        }

        if (locInput != input) {
            free(locInput);
            locInput = nullptr;
        }
        if (locOutput != output) {
            free(output);
            output = locOutput;
        }
        mbedtls_cipher_free(&this->ctx);
        return outputLen;
    }

    int encode(const uint8_t* input, size_t inputLen,
                    uint8_t*& output, size_t& outputLen) {
        if (!ERaEncrypt::base64) {
            return 0;
        }
        size_t expected = ERaEncrypt::base64EncodeExpectedLen(inputLen);
        output = (uint8_t*)ERA_MALLOC(expected + 1);
        if (output == nullptr) {
            return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
        }
        int ret = mbedtls_base64_encode(output, expected, &outputLen, input, inputLen);
        output[outputLen] = 0;
        return ret;
    }

    int decode(const uint8_t* input, size_t inputLen,
                    uint8_t*& output, size_t& outputLen) {
        if (!ERaEncrypt::base64) {
            return 0;
        }
        size_t expected = ERaEncrypt::base64DecodeExpectedLen(inputLen);
        output = (uint8_t*)ERA_MALLOC(expected + 1);
        if (output == nullptr) {
            return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
        }
        int ret = mbedtls_base64_decode(output, expected, &outputLen, input, inputLen);
        output[outputLen] = 0;
        return ret;
    }

    uint8_t secretKey[16] {0};
    const uint8_t iv[16] {'E', 'R', 'a', 'B', 'l', 'u', 'e', 't', 'o', 'o', 't', 'h', '2', '0', '2', '3'};
    mbedtls_cipher_context_t ctx;
};

#endif /* INC_ERA_ENCRYPT_MBEDTLS_HPP_ */
