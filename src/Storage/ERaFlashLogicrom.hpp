#ifndef INC_ERA_FLASH_LOGICROM_HPP_
#define INC_ERA_FLASH_LOGICROM_HPP_

#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <param.h>
#include <sys/stat.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : mFd(-1)
    {}
    ~ERaFlash()
    {}

    void begin() {}
    void end() {}
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
private:
    void mkdir(const char* path);

    int mFd;
};

inline
void ERaFlash::beginRead(const char* filename) {
    this->endRead();
    this->mFd = open(filename, O_RDONLY);
}

inline
char* ERaFlash::readLine() {
    if (this->mFd < 0) {
        return nullptr;
    }
    uint8_t c {0};
    ssize_t ret {0};
    size_t pos {0};
    size_t size = 1024;
    char* buffer = (char*)ERA_MALLOC(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    do {
        ret = read(this->mFd, &c, 1);
        if ((ret > 0) && (c != '\n')) {
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
    } while ((ret > 0) && (c != '\n'));
    buffer[pos] = 0;
    if (!pos) {
        free(buffer);
        buffer = nullptr;
    }
    return buffer;
}

inline
void ERaFlash::endRead() {
    if (this->mFd < 0) {
        return;
    }
    close(this->mFd);
    this->mFd = -1;
}

inline
void ERaFlash::beginWrite(const char* filename) {
    this->endWrite();
    this->mFd = open(filename, O_WRONLY);
    if (this->mFd < 0) {
        this->mkdir(filename);
        this->mFd = open(filename, O_WRONLY);
    }
}

inline
void ERaFlash::writeLine(const char* buf) {
    if (buf == nullptr) {
        return;
    }
    if (this->mFd < 0) {
        return;
    }
    write(this->mFd, buf, strlen(buf));
}

inline
void ERaFlash::endWrite() {
    if (this->mFd < 0) {
        return;
    }
    close(this->mFd);
    this->mFd = -1;
}

inline
char* ERaFlash::readFlash(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return nullptr;
    }
    size_t size = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);
    if (!size) {
        close(fd);
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        close(fd);
        return nullptr;
    }
    buf[size] = '\0';
    read(fd, buf, size);
    close(fd);
    return buf;
}

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    if (buf == nullptr) {
        return 0;
    }
    param_load(key, buf, maxLen);
    return maxLen;
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    int fd = open(filename, O_WRONLY);
    if (fd < 0) {
        this->mkdir(filename);
        fd = open(filename, O_WRONLY);
        if (fd < 0) {
            return;
        }
    }
    write(fd, buf, strlen(buf));
    close(fd);
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (value == nullptr) {
        return 0;
    }
    param_save(key, (void*)value, len);
    return len;
}

inline
void ERaFlash::mkdir(const char* path) {
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

#endif /* INC_ERA_FLASH_LOGICROM_HPP_ */
