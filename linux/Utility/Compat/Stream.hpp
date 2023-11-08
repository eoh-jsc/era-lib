#ifndef INC_STREAM_HPP_
#define INC_STREAM_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

class Stream
{
public:
    Stream()
        : timeout(1000UL)
        , startMillis(0UL)
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

    void setTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

    unsigned long getTimeout() {
        return this->timeout;
    }

    int timedRead() {
        int c {0};
        this->startMillis = this->unix_millis();
        do {
            c = this->read();
            if (c >= 0) {
                return c;
            }
        } while ((this->unix_millis() - this->startMillis) < this->timeout);
        return -1;
    }

    int timedPeek() {
        int c {0};
        this->startMillis = this->unix_millis();
        do {
            c = this->peek();
            if (c >= 0) {
                return c;
            }
        } while ((this->unix_millis() - this->startMillis) < this->timeout);
        return -1;
    }

    virtual size_t readBytes(uint8_t* buffer, size_t size) {
        if (buffer == NULL) {
            return 0;
        }
        size_t count {0};
        while (count < size) {
            int c = this->timedRead();
            if (c < 0) {
                break;
            }
            *buffer++ = (uint8_t)c;
            count++;
        }
        return count;
    }

    virtual size_t readBytes(char* buffer, size_t size) {
        return this->readBytes((uint8_t*)buffer, size);
    }

    virtual size_t readBytesUntil(char terminator, uint8_t* buffer, size_t size) {
        if (buffer == NULL) {
            return 0;
        }
        if (!size) {
            return 0;
        }
        size_t index {0};
        while (index < size) {
            int c = this->timedRead();
            if ((c < 0) || (c == terminator)) {
                buffer[index] = 0;
                break;
            }
            *buffer++ = (uint8_t)c;
            index++;
        }
        return index;
    }

    virtual size_t readBytesUntil(char terminator, char* buffer, size_t size) {
        return this->readBytesUntil(terminator, (uint8_t*)buffer, size);
    }

protected:
    __attribute__((constructor))
    static inline
    void unix_startup_time() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        Stream::startup_time() = (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L);
    }

    static inline
    unsigned long& startup_time() {
        static unsigned long _startup_time {0};
        return _startup_time;
    }

    unsigned long unix_millis() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ((ts.tv_sec * 1000 + ts.tv_nsec / 1000000L) - this->startup_time());
    }

    unsigned long timeout;
    unsigned long startMillis;
};

#endif /* INC_STREAM_HPP_ */
