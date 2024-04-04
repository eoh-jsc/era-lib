#ifndef INC_ERA_FLASH_LINUX_HPP_
#define INC_ERA_FLASH_LINUX_HPP_

#include <stddef.h>
#include <sys/stat.h>

class ERaFlashLinux
{
    const char* TAG = "Flash";
public:
    ERaFlashLinux()
        : mFile(nullptr)
    {}
    ~ERaFlashLinux()
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
    void writeFlash(const char *filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    void mkdir(const char* path);

    FILE* mFile;
};

inline
void ERaFlashLinux::begin() {
}

inline
void ERaFlashLinux::end() {
}

inline
void ERaFlashLinux::beginRead(const char* filename) {
    this->endRead();
    this->mFile = fopen(filename, "r");
}

inline
char* ERaFlashLinux::readLine() {
    if (this->mFile == nullptr) {
        return nullptr;
    }
    int c {0};
    size_t pos {0};
    size_t size = 1024;
    char* buffer = (char*)ERA_MALLOC(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    do {
        c = fgetc(this->mFile);
        if ((c != EOF) && (c != '\n')) {
            buffer[pos++] = (char)c;
        }
        if (pos >= size - 1) {
            size += size;
            char* copy = (char*)ERA_REALLOC(buffer, size);
            if (copy == nullptr) {
                free(buffer);
                buffer = nullptr;
                return nullptr;
            }
            buffer = copy;
        }
    } while ((c != EOF) && (c != '\n'));
    buffer[pos] = 0;
    if (!pos) {
        free(buffer);
        buffer = nullptr;
    }
    return buffer;
}

inline
void ERaFlashLinux::endRead() {
    if (this->mFile == nullptr) {
        return;
    }
    fclose(this->mFile);
    this->mFile = nullptr;
}

inline
void ERaFlashLinux::beginWrite(const char* filename) {
    this->endWrite();
    this->mFile = fopen(filename, "w");
    if (this->mFile == nullptr) {
        this->mkdir(filename);
        this->mFile = fopen(filename, "w");
    }
}

inline
void ERaFlashLinux::writeLine(const char* buf) {
    if (buf == nullptr) {
        return;
    }
    if (this->mFile == nullptr) {
        return;
    }
    fprintf(this->mFile, "%s\n", buf);
}

inline
void ERaFlashLinux::endWrite() {
    if (this->mFile == nullptr) {
        return;
    }
    fclose(this->mFile);
    this->mFile = nullptr;
}

inline
char* ERaFlashLinux::readFlash(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        return nullptr;
    }
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    if (!size) {
        fclose(file);
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        fclose(file);
        return nullptr;
    }
    buf[size] = '\0';
    fgets(buf, size + 1, file);
    fclose(file);
    return buf;
}

inline
size_t ERaFlashLinux::readFlash(const char* key, void* buf, size_t maxLen) {
    if (buf == nullptr) {
        return 0;
    }
    FILE* file = fopen(key, "rb");
    if (file == nullptr) {
        return 0;
    }
    fread(buf, maxLen, 1, file);
    fclose(file);
    return maxLen;
}

inline
void ERaFlashLinux::writeFlash(const char *filename, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    FILE* file = fopen(filename, "w");
    if (file == nullptr) {
        this->mkdir(filename);
        file = fopen(filename, "w");
        if (file == nullptr) {
            return;
        }
    }
    fprintf(file, "%s", buf);
    fclose(file);
}

inline
size_t ERaFlashLinux::writeFlash(const char* key, const void* value, size_t len) {
    if (value == nullptr) {
        return 0;
    }
    FILE* file = fopen(key, "wb");
    if (file == nullptr) {
        this->mkdir(key);
        file = fopen(key, "wb");
        if (file == nullptr) {
            return 0;
        }
    }
    fwrite(value, 1, len, file);
    fclose(file);
    return len;
}

inline
void ERaFlashLinux::mkdir(const char* path) {
    if (path == nullptr) {
        return;
    }
    size_t len {0};
    char dir[256] {0};
    char* p = nullptr;
    struct stat st {0};

    snprintf(dir, sizeof(dir), "%s", path);
    len = strlen(dir);
    if (dir[len - 1] == '/') {
        dir[len - 1] = 0;
    }
    for (p = dir + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (stat(dir, &st) == -1) {
                ::mkdir(dir, 0755);
            }
            *p = '/';
        }
    }
}

typedef ERaFlashLinux ERaFlash;

#endif /* INC_ERA_FLASH_LINUX_HPP_ */
