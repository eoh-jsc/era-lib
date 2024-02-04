#ifndef INC_PRINT_HPP_
#define INC_PRINT_HPP_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

class Print
{
public:
    enum PrintBaseT
        : uint8_t {
        BASE_BIN = 2,
        BASE_OCT = 8,
        BASE_DEC = 10,
        BASE_HEX = 16
    };

public:
    Print()
    {}
    virtual ~Print()
    {}

    virtual size_t write(uint8_t) = 0;

    size_t write(const char* str) {
        if (str == NULL) {
            return 0;
        }
        return this->write((const uint8_t*)str, strlen(str));
    }

    virtual size_t write(const uint8_t* buffer, size_t size) {
        size_t n {0};
        while (size--) {
            size_t ret = this->write(*buffer++);
            if (ret == 0) {
                break;
            }
            n += ret;
        }
        return n;
    }

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
            temp = (char*)malloc(len+1);
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

    size_t print(char c) {
        return this->write((uint8_t)c);
    }

    size_t print(unsigned char num, int base = PrintBaseT::BASE_DEC) {
        return this->print((unsigned long)num, base);
    }

    size_t print(int num, int base = PrintBaseT::BASE_DEC) {
        return this->print((long)num, base);
    }

    size_t print(unsigned int num, int base = PrintBaseT::BASE_DEC) {
        return this->print((unsigned long)num, base);
    }

    size_t print(long num, int base = PrintBaseT::BASE_DEC) {
        int t {0};
        if ((base == PrintBaseT::BASE_DEC) && (num < 0)) {
            t = this->print('-');
            num = -num;
        }
        return (this->printNumber(static_cast<unsigned long>(num), base) + t);
    }

    size_t print(unsigned long num, int base = PrintBaseT::BASE_DEC) {
        if (base == 0) {
            return this->write((uint8_t)num);
        }
        else {
            return this->printNumber(num, base);
        }
    }

    size_t print(long long num, int base = PrintBaseT::BASE_DEC) {
        int t {0};
        if ((base == PrintBaseT::BASE_DEC) && (num < 0)) {
            t = this->print('-');
            num = -num;
        }
        return (this->printNumber(static_cast<unsigned long long>(num), base) + t);
    }

    size_t print(unsigned long long num, int base = PrintBaseT::BASE_DEC) {
        if (base == 0) {
            return this->write((uint8_t)num);
        }
        else {
            return this->printNumber(num, base);
        }
    }

    size_t print(double num, int digits = 2) {
        return this->printFloat(num, digits);
    }

    size_t println(char c) {
        size_t n = this->print(c);
        n += this->println();
        return n;
    }

    size_t println(unsigned char num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(int num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(unsigned int num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(long num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(unsigned long num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(long long num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(unsigned long long num, int base = PrintBaseT::BASE_DEC) {
        size_t n = this->print(num, base);
        n += this->println();
        return n;
    }

    size_t println(double num, int digits = 2) {
        size_t n = this->print(num, digits);
        n += this->println();
        return n;
    }

    virtual void flush() {
    }

protected:
private:
    size_t printNumber(unsigned long number, uint8_t base) {
        char buf[8 * sizeof(unsigned long) + 1] {0};
        char* str = &buf[sizeof(buf) - 1];

        *str = '\0';

        if (base < 2) {
            base = 10;
        }

        do {
            char c = (number % base);
            number /= base;

            *--str = ((c < 10) ? (c + '0') : (c + 'A' - 10));
        } while (number);

        return this->write(str);
    }

    size_t printNumber(unsigned long long number, uint8_t base) {
        char buf[8 * sizeof(unsigned long long) + 1] {0};
        char* str = &buf[sizeof(buf) - 1];

        *str = '\0';

        if (base < 2) {
            base = 10;
        }

        do {
            auto m = number;
            number /= base;
            char c = (m - base * number);

            *--str = ((c < 10) ? (c + '0') : (c + 'A' - 10));
        } while (number);

        return this->write(str);
    }

    size_t printFloat(double number, uint8_t digits) {
        size_t n {0};

        if (isnan(number)) {
            return this->print("nan");
        }
        if (isinf(number)) {
            return this->print("inf");
        }
        if (number > 4294967040.0) {
            return this->print("ovf");
        }
        if (number < -4294967040.0) {
            return this->print("ovf");
        }

        if (number < 0.0) {
            n += this->print('-');
            number = -number;
        }

        double rounding = 0.5;
        for (uint8_t i = 0; i < digits; ++i) {
            rounding /= 10.0;
        }

        number += rounding;

        unsigned long int_part = (unsigned long) number;
        double remainder = number - (double) int_part;
        n += this->print(int_part);

        if (digits > 0) {
            n += this->print(".");
        }

        while (digits-- > 0) {
            remainder *= 10.0;
            int toPrint = int(remainder);
            n += this->print(toPrint);
            remainder -= toPrint;
        }

        return n;
    }

};

#endif /* INC_PRINT_HPP_ */
