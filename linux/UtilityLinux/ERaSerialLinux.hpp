#ifndef INC_ERA_SERIAL_LINUX_HPP_
#define INC_ERA_SERIAL_LINUX_HPP_

#if defined(LINUX) && defined(RASPBERRY)
    #include <wiringSerial.h>
#elif defined(LINUX)
    #include "Compat/SerialLinux.hpp"
#endif

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

    void begin(const char *device, const int baud) {
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

    int available() {
        if (!this->connected()) {
            return 0;
        }
        return serialDataAvail(this->fd);
    }

    int read() {
        if (!this->connected()) {
            return -1;
        }
        return serialGetchar(this->fd);
    }

    int peek() {
        return -1;
    }

    size_t write(uint8_t value) {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buffer, size_t size) {
        if (!this->connected()) {
            return 0;
        }
        for (size_t i = 0; i < size; ++i) {
            serialPutchar(this->fd, buffer[i]);
        }
        return size;
    }

    void flush() {
        if (!this->connected()) {
            return;
        }
        serialFlush(this->fd);
    }

private:
    bool connected() {
        return (this->fd >= 0);
    }

    int fd;
};

#endif /* INC_ERA_SERIAL_LINUX_HPP_ */
