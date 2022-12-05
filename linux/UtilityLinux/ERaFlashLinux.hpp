#ifndef INC_ERA_FLASH_LINUX_HPP_
#define INC_ERA_FLASH_LINUX_HPP_

#include <stddef.h>
#include <sys/stat.h>

class ERaFlashLinux
{
    const char* TAG = "Flash";
public:
    ERaFlashLinux()
    {}
    ~ERaFlashLinux()
    {}

    void begin();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char *filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    void mkdir(const char* path);
};

void ERaFlashLinux::begin() {
}

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
    fprintf(file, buf);
    fclose(file);
}

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
    fwrite(value, len, 1, file);
    fclose(file);
    return len;
}

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

#endif /* INC_ERA_FLASH_LINUX_HPP_ */
