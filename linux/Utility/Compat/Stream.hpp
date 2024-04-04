#ifndef INC_STREAM_HPP_
#define INC_STREAM_HPP_

#include <time.h>
#include <sys/time.h>
#include "Print.hpp"

class ERaString;

class Stream
    : public Print
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

    int peekNextDigit() {
        int c {0};
        while(true) {
            c = this->timedPeek();
            if (c < 0) {
                return c;    // timeout
            }
            if (c == '-') {
                return c;
            }
            if ((c >= '0') && (c <= '9')) {
                return c;
            }
            this->read();  // discard non-numeric
        }
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

    virtual ERaString readString();
    ERaString readStringUntil(char terminator);

    long parseInt() {
        return this->parseInt(NO_SKIP_CHAR);
    }

    long parseInt(char skipChar) {
        bool isNegative {false};
        long value {0};
        int c {0};

        c = this->peekNextDigit();
        if (c < 0) {
            return 0;
        }

        do {
            if (c == skipChar) {
            }
            else if (c == '-') {
                isNegative = true;
            }
            else if ((c >= '0') && (c <= '9')) {
                value = ((value * 10) + (c - '0'));
            }
            this->read();
            c = this->timedPeek();
        } while (((c >= '0') && (c <= '9')) || (c == skipChar));

        if (isNegative) {
            value = -value;
        }
        return value;
    }

    float parseFloat() {
        return this->parseFloat(NO_SKIP_CHAR);
    }

    float parseFloat(char skipChar) {
        bool isNegative {false};
        bool isFraction {false};
        long value {0};
        int c;
        float fraction {1.0f};

        c = this->peekNextDigit();
        if (c < 0) {
            return 0;
        }

        do {
            if (c == skipChar) {
            }
            else if (c == '-') {
                isNegative = true;
            }
            else if (c == '.') {
                isFraction = true;
            }
            else if ((c >= '0') && (c <= '9')) {
                value = ((value * 10) + (c - '0'));
                if (isFraction) {
                    fraction *= 0.1f;
                }
            }
            this->read();
            c = this->timedPeek();
        } while (((c >= '0') && (c <= '9')) || (c == '.') || (c == skipChar));

        if (isNegative) {
            value = -value;
        }
        if (isFraction) {
            return (value * fraction);
        }
        else {
            return value;
        }
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

    const char NO_SKIP_CHAR = 1;
};

#endif /* INC_STREAM_HPP_ */
