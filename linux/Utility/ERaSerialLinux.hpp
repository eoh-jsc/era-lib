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
    {}
    ~ERaSerialLinux()
    {}

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
    }

    int available() override {
        if (!this->connected()) {
            return 0;
        }
        return serialDataAvail(this->fd);
    }

    int read() override {
        if (!this->connected()) {
            return -1;
        }
        return serialGetchar(this->fd);
    }

    int peek() override {
        return -1;
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
};

typedef ERaSerialLinux ERaSerial;

#endif /* INC_ERA_SERIAL_LINUX_HPP_ */
