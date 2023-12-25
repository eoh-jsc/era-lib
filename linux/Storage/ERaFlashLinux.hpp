#ifndef INC_ERA_FLASH_LINUX_HPP_
#define INC_ERA_FLASH_LINUX_HPP_

#include <stddef.h>
#include <sys/stat.h>

class ERaFlashLinux
{
    const char* TAG = "Flash";
public:
    ERaFlashLinux()
        : file(nullptr)
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

    FILE* file;
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
    this->file = fopen(filename, "r");
}

inline
char* ERaFlashLinux::readLine() {
    if (this->file == nullptr) {
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
        c = fgetc(this->file);
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
    if (this->file == nullptr) {
        return;
    }
    fclose(this->file);
    this->file = nullptr;
}

inline
void ERaFlashLinux::beginWrite(const char* filename) {
    this->endWrite();
    this->file = fopen(filename, "w");
    if (this->file == nullptr) {
        this->mkdir(filename);
        this->file = fopen(filename, "w");
    }
}

inline
void ERaFlashLinux::writeLine(const char* buf) {
    if (buf == nullptr) {
        return;
    }
    if (this->file == nullptr) {
        return;
    }
    fprintf(this->file, "%s\n", buf);
}

inline
void ERaFlashLinux::endWrite() {
    if (this->file == nullptr) {
        return;
    }
    fclose(this->file);
    this->file = nullptr;
}

inline
char* ERaFlashLinux::readFlash(const char* filename) {
    FILE* _file = fopen(filename, "r");
    if (_file == nullptr) {
        return nullptr;
    }
    fseek(_file, 0L, SEEK_END);
    size_t size = ftell(_file);
    fseek(_file, 0L, SEEK_SET);
    if (!size) {
        fclose(_file);
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        fclose(_file);
        return nullptr;
    }
    buf[size] = '\0';
    fgets(buf, size + 1, _file);
    fclose(_file);
    return buf;
}

inline
size_t ERaFlashLinux::readFlash(const char* key, void* buf, size_t maxLen) {
    if (buf == nullptr) {
        return 0;
    }
    FILE* _file = fopen(key, "rb");
    if (_file == nullptr) {
        return 0;
    }
    fread(buf, maxLen, 1, _file);
    fclose(_file);
    return maxLen;
}

inline
void ERaFlashLinux::writeFlash(const char *filename, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    FILE* _file = fopen(filename, "w");
    if (_file == nullptr) {
        this->mkdir(filename);
        _file = fopen(filename, "w");
        if (_file == nullptr) {
            return;
        }
    }
    fprintf(_file, "%s", buf);
    fclose(_file);
}

inline
size_t ERaFlashLinux::writeFlash(const char* key, const void* value, size_t len) {
    if (value == nullptr) {
        return 0;
    }
    FILE* _file = fopen(key, "wb");
    if (_file == nullptr) {
        this->mkdir(key);
        _file = fopen(key, "wb");
        if (_file == nullptr) {
            return 0;
        }
    }
    fwrite(value, 1, len, _file);
    fclose(_file);
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
