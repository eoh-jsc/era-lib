#ifndef INC_ERA_SERIAL_LINUX_HPP_
#define INC_ERA_SERIAL_LINUX_HPP_

#if defined(LINUX) &&        \
    (defined(RASPBERRY) ||   \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI))
    #include <wiringSerial.h>
#elif defined(LINUX)
    #include "Compat/SerialLinux.hpp"
#endif

#define DTR_PIN TIOCM_DTR
#define RTS_PIN TIOCM_RTS

#include "Compat/Stream.hpp"

class ERaSerialLinux
    : public Stream
{
public:
    ERaSerialLinux()
        : fd(-1)
        , hasPeek(false)
        , peekByte(0)
    {}
    ERaSerialLinux(int _fd)
        : fd(_fd)
        , hasPeek(false)
        , peekByte(0)
    {}
    ~ERaSerialLinux()
    {
        this->end();
    }

    void begin(const char* device, const int baud) {
        this->end();
        this->fd = serialOpen(device, baud);
    }

    void end() {
        if (!this->connected()) {
            return;
        }
        this->flush();
        serialClose(this->fd);
        this->fd = -1;
    }

    int available() override {
        if (!this->connected()) {
            return 0;
        }
        int result {0};
        result = serialDataAvail(this->fd);
        if (result < 0) {
            return (this->hasPeek ? 1 : 0);
        }
        if (this->hasPeek) {
            result++;
        }
        return result;
    }

    int read() override {
        if (!this->connected()) {
            return -1;
        }
        if (this->hasPeek) {
            this->hasPeek = false;
            return this->peekByte;
        }
        return serialGetchar(this->fd);
    }

    int peek() override {
        if (!this->available()) {
            return -1;
        }
        uint8_t byte {0};
        if (this->hasPeek) {
            byte = this->peekByte;
        }
        else {
            int c = serialGetchar(this->fd);
            if (c < 0) {
                return -1;
            }
            byte = (uint8_t)c;
            this->hasPeek = true;
            this->peekByte = byte;
        }
        return byte;
    }

    size_t write(uint8_t value) override {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        if (!this->connected()) {
            return 0;
        }
        for (size_t i = 0; i < size; ++i) {
            serialPutchar(this->fd, buffer[i]);
        }
        return size;
    }

    void flush() override {
        if (!this->connected()) {
            return;
        }
        serialFlush(this->fd);
    }

    operator int() const override {
        return this->fd;
    }

private:
    bool connected() {
        return (this->fd >= 0);
    }

    int fd;
    bool hasPeek;
    uint8_t peekByte;
};

typedef ERaSerialLinux ERaSerial;
typedef ERaSerialLinux HardwareSerial;

#endif /* INC_ERA_SERIAL_LINUX_HPP_ */
