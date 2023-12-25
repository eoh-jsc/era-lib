/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security,
 * Inc. MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Written by Solar Designer <solar at openwall.com> in 2001, and placed
 * in the public domain.  There's absolutely no warranty.
 *
 * This differs from Colin Plumb's older public domain implementation in
 * that no 32-bit integer data type is required, there's no compile-time
 * endianness configuration, and the function prototypes match OpenSSL's.
 * The primary goals are portability and ease of use.
 *
 * This implementation is meant to be fast, but not as fast as possible.
 * Some known optimizations are not included to reduce source code size
 * and avoid compile-time configuration.
 */

#ifndef INC_MD5_HPP_
#define INC_MD5_HPP_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long MD5_u32plus_t;

typedef struct __MD5_CTX_t {
    MD5_u32plus_t lo, hi;
    MD5_u32plus_t a, b, c, d;
    unsigned char buffer[64];
    MD5_u32plus_t block[16];
} MD5_CTX_t;

class MD5
{
public:
    MD5()
        : pCtx(NULL)
    {
        memset(this->state, 0, sizeof(this->state));
    }
    ~MD5()
    {
        if (this->pCtx != NULL) {
            delete this->pCtx;
        }
        this->pCtx = NULL;
    }

    void begin() {
        this->init();
    }

    void init() {
        if (this->pCtx == NULL) {
            this->pCtx = new MD5_CTX_t;
            if (this->pCtx == NULL) {
                return;
            }
        }

        MD5::MD5Init(this->pCtx);
    }

    void update(const void* data, size_t size) {
        if (this->pCtx == NULL) {
            return;
        }

        MD5::MD5Update(this->pCtx, data, size);
    }

    void update(const char* data) {
        this->update(data, strlen(data));
    }

    template <typename T>
    void update(const T& data) {
        this->update(&data, 1);
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

    char* finalize() {
        if (this->pCtx == NULL) {
            return NULL;
        }

        unsigned char hash[16] {0};
        MD5::MD5Final(hash, this->pCtx);
        return MD5::makeDigest(hash, sizeof(hash), this->state);
    }

    static unsigned char* makeHash(char* arg);
    static unsigned char* makeHash(char* arg, size_t size);
    static char* makeDigest(const unsigned char* digest, int len, char* result = NULL);
     static const void* body(void* ctxBuf, const void* data, size_t size);
    static void MD5Init(void* ctxBuf);
    static void MD5Final(unsigned char* result, void* ctxBuf);
    static void MD5Update(void* ctxBuf, const void* data, size_t size);

private:
    MD5_CTX_t* pCtx;
    char state[33];
};

#endif /* INC_MD5_HPP_ */
