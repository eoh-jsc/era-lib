#ifndef INC_ERA_ENCRYPT_BEARSSL_HPP_
#define INC_ERA_ENCRYPT_BEARSSL_HPP_

#include <string.h>
#include <bearssl/bearssl.h>
#include <ERa/ERaEncrypt.hpp>
#include <ERa/ERaDefine.hpp>
#include <Utility/ERaLoc.hpp>
#include <Utility/Base64.hpp>

class ERaEncryptBearSSL
    : public ERaEncrypt
{
public:
    ERaEncryptBearSSL(bool _base64 = false)
        : ERaEncrypt(_base64)
    {}
    ~ERaEncryptBearSSL()
    {}

    void begin(const uint8_t* key,
        EncryptModeT modeEnc = EncryptModeT::ENCRYPT_MODE_CBC) override {
        this->mode = modeEnc;
        this->setSecretKey(key);
    }

    void setSecretKey(const uint8_t* key) override {
        memcpy(this->secretKey, key, sizeof(this->secretKey));
    }

    void setInitializationVector(const uint8_t* nonce,
                                size_t nonceLen) override {
        this->ivLen = nonceLen;
        memset(this->iv, 0, sizeof(this->iv));
        memcpy(this->iv, nonce, nonceLen);
    }

    size_t encrypt(const uint8_t* input, size_t inputLen,
                uint8_t*& output, size_t& outputLen) override {
        size_t nBlock = inputLen / 16 + 1;
        size_t bufLen = nBlock * 16;
        uint8_t nPadding = nBlock * 16 - inputLen;

        uint8_t* buf = (uint8_t*)ERA_MALLOC(bufLen + 1);
        if (buf == nullptr) {
            return 0;
        }

        buf[bufLen] = 0;
        memcpy(buf, input, inputLen);
        for (size_t i = inputLen; i < bufLen; ++i) {
            buf[i] = nPadding;
        }
        uint8_t _iv[16] {0};
        memcpy(_iv, this->iv, this->ivLen);

        switch (this->mode) {
            case EncryptModeT::ENCRYPT_MODE_CCM: {
                br_ccm_context ctx;
                const br_block_ctrcbc_class* bCtx = &br_aes_big_ctrcbc_vtable;
                br_aes_big_ctrcbc_vtable.init(&bCtx, this->secretKey, sizeof(this->secretKey));
                br_ccm_init(&ctx, &bCtx);
                br_ccm_reset(&ctx, _iv, this->ivLen, 0, inputLen, 16);
                br_ccm_run(&ctx, 1, buf, inputLen);
                br_ccm_get_tag(&ctx, buf + inputLen);
            }
                break;
            case EncryptModeT::ENCRYPT_MODE_CBC:
            default: {
                br_aes_big_cbcenc_keys ctx;
                br_aes_big_cbcenc_init(&ctx, this->secretKey, sizeof(this->secretKey));
                br_aes_big_cbcenc_run(&ctx, _iv, buf, bufLen);
            }
                break;
        }

        output = buf;
        outputLen = bufLen;
        this->encode(buf, bufLen, output, outputLen);

        if (buf != output) {
            free(buf);
            buf = nullptr;
        }
        return outputLen;
    }

    size_t decrypt(const uint8_t* input, size_t inputLen,
                uint8_t*& output, size_t& outputLen) override {
        uint8_t* buf = nullptr;
        size_t bufLen = 0;
        size_t nBlock = 0;

        this->decode(input, inputLen, buf, bufLen);
        if (!bufLen || (buf == nullptr)) {
            bufLen = inputLen;
            buf = (uint8_t*)ERA_MALLOC(bufLen + 1);
            if (buf == nullptr) {
                return 0;
            }
            buf[bufLen] = 0;
            memcpy(buf, input, inputLen);
        }
        nBlock = bufLen / 16;
        outputLen = nBlock * 16;

        uint8_t _iv[16] {0};
        memcpy(_iv, this->iv, this->ivLen);

        switch (this->mode) {
            case EncryptModeT::ENCRYPT_MODE_CCM: {
                br_ccm_context ctx;
                const br_block_ctrcbc_class* bCtx = &br_aes_big_ctrcbc_vtable;
                br_aes_big_ctrcbc_vtable.init(&bCtx, this->secretKey, sizeof(this->secretKey));
                br_ccm_init(&ctx, &bCtx);
                br_ccm_reset(&ctx, _iv, this->ivLen, 0, bufLen, 16);
                br_ccm_run(&ctx, 0, buf, bufLen);
                br_ccm_check_tag(&ctx, buf + bufLen);
            }
                break;
            case EncryptModeT::ENCRYPT_MODE_CBC:
            default: {
                br_aes_big_cbcdec_keys ctx;
                br_aes_big_cbcdec_init(&ctx, this->secretKey, sizeof(this->secretKey));
                br_aes_big_cbcdec_run(&ctx, _iv, buf, outputLen);
            }
                break;
        }

        uint8_t nPadding = buf[outputLen - 1];
        outputLen -= nPadding;
        buf[outputLen] = 0;
        output = buf;
        return outputLen;
    }

protected:
private:
    int encode(const uint8_t* input, size_t inputLen,
                    uint8_t*& output, size_t& outputLen) {
        if (!ERaEncrypt::base64) {
            return 0;
        }
        size_t expected = ERaEncrypt::base64EncodeExpectedLen(inputLen);
        output = (uint8_t*)ERA_MALLOC(expected + 1);
        if (output == nullptr) {
            return -1;
        }
        outputLen = base64_encode((char*)output, (char*)input, inputLen);
        output[outputLen] = 0;
        return 0;
    }

    int decode(const uint8_t* input, size_t inputLen,
                    uint8_t*& output, size_t& outputLen) {
        if (!ERaEncrypt::base64) {
            return 0;
        }
        size_t expected = ERaEncrypt::base64DecodeExpectedLen(inputLen);
        output = (uint8_t*)ERA_MALLOC(expected + 1);
        if (output == nullptr) {
            return -1;
        }
        outputLen = base64_decode((char*)output, (char*)input, inputLen);
        output[outputLen] = 0;
        return 0;
    }

    uint8_t secretKey[16] {0};
    uint8_t iv[16] {'E', 'R', 'a', 'B', 'l', 'u', 'e', 't', 'o', 'o', 't', 'h', '2', '0', '2', '3'};
    uint8_t ivLen {16};
    br_aes_big_cbcdec_keys ctx;
};

#endif /* INC_ERA_ENCRYPT_BEARSSL_HPP_ */
