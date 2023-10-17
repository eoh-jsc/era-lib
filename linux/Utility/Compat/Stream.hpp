#ifndef INC_STREAM_HPP_
#define INC_STREAM_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
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
        if (str == NULL) {
            return 0;
        }
        return this->write((const uint8_t*)str, strlen(str));
    }

    virtual size_t write(const uint8_t* buffer, size_t size) = 0;

    size_t write(const char* buffer, size_t size) {
        return this->write((const uint8_t*)buffer, size);
    }

    size_t print(const char* str) {
        return this->write(str);
    }

    size_t println() {
        return this->print("\r\n");
    }

    size_t println(const char* str) {
        size_t n = this->print(str);
        n += this->println();
        return n;
    }

    size_t printf(const char *format, ...) {
        char loc_buf[64];
        char * temp = loc_buf;
        va_list arg;
        va_list copy;
        va_start(arg, format);
        va_copy(copy, arg);
        int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
        va_end(copy);
        if (len < 0) {
            va_end(arg);
            return 0;
        };
        if (len >= (int)sizeof(loc_buf)){
            temp = (char*) malloc(len+1);
            if (temp == NULL) {
                va_end(arg);
                return 0;
            }
            len = vsnprintf(temp, len+1, format, arg);
        }
        va_end(arg);
        len = this->write((uint8_t*)temp, len);
        if (temp != loc_buf){
            free(temp);
        }
        return len;
    }

    virtual void flush() = 0;

    virtual operator int() const {
        return -1;
    }

protected:
};

#endif /* INC_STREAM_HPP_ */
