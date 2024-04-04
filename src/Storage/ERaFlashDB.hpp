#ifndef INC_ERA_FLASH_DB_HPP_
#define INC_ERA_FLASH_DB_HPP_

#include <stdio.h>
#include <string.h>
#include <flashdb.h>
#include <Utility/CRC32.hpp>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : mInitialized(false)
        , mFileName{}
        , mLine(0)
    {}
    ~ERaFlash()
    {}

    void begin();
    void end();
    void beginRead(const char* filename);
    char* readLine();
    void endRead();
    void beginWrite(const char* filename);
    void writeLine(const char* buf);
    void endWrite();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char* filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
    String encodeUnique(uint32_t number, size_t len);

private:
    char* readString(const char* key);
    void writeString(const char* key, const char* buf);

    struct fdb_kvdb mDb;
    struct fdb_blob mBlob;
    bool mInitialized;
    char mFileName[16];
    size_t mLine;
};

inline
void ERaFlash::begin() {
    if (this->mInitialized) {
        return;
    }
    this->end();
    fdb_err_t err = fdb_kvdb_init(&this->mDb, "era", "kvs", NULL, NULL);
    if (err != FDB_NO_ERR) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("ERa flash init FAILED"));
        return;
    }
    this->mInitialized = true;
}

inline
void ERaFlash::end() {
    fdb_kv_set_default(&this->mDb);
    fdb_kvdb_deinit(&this->mDb);
    this->mInitialized = false;
}

inline
void ERaFlash::beginRead(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endRead();

    uint32_t number = CRC32::calculate(filename, strlen(filename));
    String key = this->encodeUnique(number, 8);
    key.toCharArray(this->mFileName, sizeof(this->mFileName));
}

inline
char* ERaFlash::readLine() {
    if (!this->mInitialized) {
        return nullptr;
    }

    char key[32] {0};
    snprintf(key, sizeof(key), "%s%02d", this->mFileName, this->mLine++);

    return this->readString(key);
}

inline
void ERaFlash::endRead() {
    if (!this->mInitialized) {
        return;
    }

    this->mLine = 0;
    memset(this->mFileName, 0, sizeof(this->mFileName));
}

inline
void ERaFlash::beginWrite(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endWrite();

    uint32_t number = CRC32::calculate(filename, strlen(filename));
    String key = this->encodeUnique(number, 8);
    key.toCharArray(this->mFileName, sizeof(this->mFileName));
}

inline
void ERaFlash::writeLine(const char* buf) {
    if (!this->mInitialized) {
        return;
    }

    char key[32] {0};
    snprintf(key, sizeof(key), "%s%02d", this->mFileName, this->mLine++);

    return this->writeString(key, buf);
}

inline
void ERaFlash::endWrite() {
    if (!this->mInitialized) {
        return;
    }

    this->mLine = 0;
    memset(this->mFileName, 0, sizeof(this->mFileName));
}

inline
char* ERaFlash::readFlash(const char* filename) {
    if (!this->mInitialized) {
        return nullptr;
    }

    uint32_t number = CRC32::calculate(filename, strlen(filename));
    String key = this->encodeUnique(number, 8);
    return this->readString(key.c_str());
}

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    if (!this->mInitialized) {
        return 0;
    }

    struct fdb_kv kv;
    fdb_kv_t kvp = fdb_kv_get_obj(&this->mDb, key, &kv);
    if (kvp == nullptr) {
        return 0;
    }
    if (!kv.value_len) {
        return 0;
    }
    maxLen = ERaMin(maxLen, (size_t)kv.value_len);
    fdb_blob_make(&this->mBlob, buf, maxLen);
    size_t actualLen = fdb_kv_get_blob(&this->mDb, key, &this->mBlob);
    return actualLen;
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (!this->mInitialized) {
        return;
    }

    if (buf == nullptr) {
        return;
    }
    uint32_t number = CRC32::calculate(filename, strlen(filename));
    String key = this->encodeUnique(number, 8);
    this->writeString(key.c_str(), buf);
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (!this->mInitialized) {
        return 0;
    }

    fdb_blob_make(&this->mBlob, value, len);
    fdb_err_t err = fdb_kv_set_blob(&this->mDb, key, &this->mBlob);
    if (err != FDB_NO_ERR) {
        return 0;
    }
    return len;
}

inline
char* ERaFlash::readString(const char* key) {
    struct fdb_kv kv;
    fdb_kv_t kvp = fdb_kv_get_obj(&this->mDb, key, &kv);
    if (kvp == nullptr) {
        return nullptr;
    }
    if (!kv.value_len) {
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(kv.value_len + 1);
    if (buf == nullptr) {
        return nullptr;
    }
    buf[kv.value_len] = '\0';
    fdb_blob_make(&this->mBlob, buf, kv.value_len);
    fdb_kv_get_blob(&this->mDb, key, &this->mBlob);
    return buf;
}

inline
void ERaFlash::writeString(const char* key, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    fdb_blob_make(&this->mBlob, buf, strlen(buf));
    fdb_kv_set_blob(&this->mDb, key, &this->mBlob);
}

inline
String ERaFlash::encodeUnique(uint32_t number, size_t len) {
    static constexpr char alphabet[] { "MJGH27ARD98PS6QMOI95A5VK4E3WI4RT" };
    static constexpr int base = (sizeof(alphabet) - 1);

    char buf[16] {0};
    char prev {0};
    for (size_t i = 0; i < len; number /= base) {
        char c = alphabet[number % base];
        if (c == prev) {
            c = alphabet[(number + 1) % base];
        }
        prev = buf[i++] = c;
    }
    return String(buf);
}

#endif /* INC_ERA_FLASH_DB_HPP_ */
