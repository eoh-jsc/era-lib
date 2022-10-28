#ifndef INC_ERA_FLASH_ARDUINO_HPP_
#define INC_ERA_FLASH_ARDUINO_HPP_

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
    {}
    ~ERaFlash()
    {}

    void begin() {};
    char* readFlash(const char* filename) { return nullptr; };
    size_t readFlash(const char* key, void* buf, size_t maxLen) { return 0; };
    void writeFlash(const char *filename, const char* buf) {};
    size_t writeFlash(const char* key, const void* value, size_t len) { return 0; };

protected:
private:
};

#endif /* INC_ERA_FLASH_ARDUINO_HPP_ */
