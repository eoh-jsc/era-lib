#ifndef INC_STREAM_HPP_
#define INC_STREAM_HPP_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

class Stream
{
public:
    Stream()
    {}
    virtual ~Stream()
    {}

    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual size_t write(uint8_t) = 0;

    size_t write(const char* str) {
        if(str == NULL) {
            return 0;
        }
        return this->write((const uint8_t*)str, strlen(str));
    }

    virtual size_t write(const uint8_t* buffer, size_t size) = 0;

    size_t write(const char* buffer, size_t size) {
        return this->write((const uint8_t*)buffer, size);
    }

    virtual void flush() = 0;

protected:
};

#endif /* INC_STREAM_HPP_ */
