#ifndef INC_ERA_EMPTY_STREAM_HPP_
#define INC_ERA_EMPTY_STREAM_HPP_

#include <Stream.hpp>

class ERaEmptyStream
    : public Stream
{
public:
    ERaEmptyStream()
    {}
    ~ERaEmptyStream()
    {}

    virtual int available() override {
        return 0;
    }

    virtual int read() override {
        return -1;
    }

    virtual int peek() override {
        return -1;
    }

    virtual size_t readBytes(char* buffer, size_t len) {
        ERA_FORCE_UNUSED(buffer);
        ERA_FORCE_UNUSED(len);
        return 0;
    }

    virtual int read(uint8_t* buffer, size_t len) {
        ERA_FORCE_UNUSED(buffer);
        ERA_FORCE_UNUSED(len);
        return 0;
    }

    virtual size_t write(uint8_t value) override {
        ERA_FORCE_UNUSED(value);
        return 1;
    }

    virtual size_t write(const uint8_t* buffer, size_t size) override {
        ERA_FORCE_UNUSED(buffer);
        ERA_FORCE_UNUSED(size);
        return size;
    }

    virtual int availableForWrite() {
        return 4096;
    }

    virtual void flush() {

    }
};

using EmptyStream = ERaEmptyStream;

#endif /* INC_ERA_EMPTY_STREAM_HPP_ */
