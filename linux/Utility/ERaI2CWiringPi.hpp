#ifndef INC_ERA_I2C_WIRING_PI_HPP_
#define INC_ERA_I2C_WIRING_PI_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#if defined(WIRING_PI)
    #include <wiringPiI2C.h>
#endif

#include "Compat/Stream.hpp"

#define ERA_I2C_DEV

#if !defined(ERA_I2C_BUFFER_SIZE)
    #define ERA_I2C_BUFFER_SIZE ERA_MAX_READ_BYTES
#endif

enum I2CStatusT
    : uint8_t {
    I2C_STATUS_SUCCESS = 0,
    I2C_STATUS_NOT_CONNECT = 1,
    I2C_STATUS_NULL_BUFFER = 2,
    I2C_STATUS_FAILED_WRITE = 3
};

class ERaI2CWiringPi
    : public Stream
{
public:
    ERaI2CWiringPi()
        : fd(-1)
        , bufferSize(ERA_I2C_BUFFER_SIZE)
        , rxBuffer(NULL)
        , rxIndex(0)
        , rxLength(0)
        , txBuffer(NULL)
        , txLength(0)
        , txAddress(0)
        , disableMutex(false)
        , mutex(NULL)
    {}
    ERaI2CWiringPi(int _fd)
        : fd(_fd)
        , bufferSize(ERA_I2C_BUFFER_SIZE)
        , rxBuffer(NULL)
        , rxIndex(0)
        , rxLength(0)
        , txBuffer(NULL)
        , txLength(0)
        , txAddress(0)
        , disableMutex(false)
        , mutex(NULL)
    {}
    ~ERaI2CWiringPi()
    {
        this->freeWireBuffer();
        this->end();
    }

    void begin(const int devId) {
        this->end();
        this->fd = this->setup(devId);
        if (this->fd >= 0) {
            this->txLength = 0;
            this->txAddress = devId;
            this->allocateWireBuffer();
        }
    }

    void begin(const char* device, int devId) {
        this->end();
        this->fd = this->setupInterface(device, devId);
        if (this->fd >= 0) {
            this->txLength = 0;
            this->txAddress = devId;
            this->allocateWireBuffer();
        }
    }

    void end() {
        if (!this->connected()) {
            return;
        }
        this->flush();
        ::close(this->fd);
        this->fd = -1;
    }

    void disableLocks(bool disable = true) {
        this->disableMutex = disable;
    }

    int available() override {
        if (!this->connected()) {
            return 0;
        }
        int result = (this->rxLength - this->rxIndex);
        return result;
    }

    int read() override {
        if (!this->connected()) {
            return -1;
        }
        if (this->rxBuffer == NULL) {
            return -1;
        }
        int value {-1};
        if (this->rxIndex < this->rxLength) {
            value = this->rxBuffer[this->rxIndex++];
        }
        return value;
    }

    int peek() override {
        if (!this->connected()) {
            return -1;
        }
        if (this->rxBuffer == NULL) {
            return -1;
        }
        int value {-1};
        if (this->rxIndex < this->rxLength) {
            value = this->rxBuffer[this->rxIndex];
        }
        return value;
    }

    size_t write(uint8_t value) override {
        if (!this->connected()) {
            return 0;
        }
        if (this->txBuffer == NULL) {
            return 0;
        }
        if (this->txLength >= this->bufferSize) {
            return 0;
        }
        this->txBuffer[this->txLength++] = value;
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        for (size_t i = 0; i < size; ++i) {
            if (!this->write(buffer[i])) {
                return i;
            }
        }
        return size;
    }

    void flush() override {
        this->rxIndex = 0;
        this->rxLength = 0;
        this->txLength = 0;
    }

    inline
    size_t write(const char* buffer) {
        if (buffer == NULL) {
            return 0;
        }
        return this->write((uint8_t*)buffer, strlen(buffer));
    }

    inline
    size_t write(int value) {
        return this->write((uint8_t)value);
    }

    inline
    size_t write(unsigned int value) {
        return this->write((uint8_t)value);
    }

    inline
    size_t write(long value) {
        return this->write((uint8_t)value);
    }

    inline
    size_t write(unsigned long value) {
        return this->write((uint8_t)value);
    }

    operator int() const override {
        return this->fd;
    }

    bool allocateWireBuffer() {
        if (this->rxBuffer == NULL) {
            this->rxBuffer = (uint8_t*)malloc(this->bufferSize);
            if (this->rxBuffer == NULL) {
                return false;
            }
        }
        if (this->txBuffer == NULL) {
            this->txBuffer = (uint8_t*)malloc(this->bufferSize);
            if (this->txBuffer == NULL) {
                return false;
            }
        }
        return true;
    }

    void freeWireBuffer() {
        if (this->rxBuffer != NULL) {
            free(this->rxBuffer);
            this->rxBuffer = NULL;
        }
        if (this->txBuffer != NULL) {
            free(this->txBuffer);
            this->txBuffer = NULL;
        }
    }

    void beginTransmission(const int devId) {
        this->lockI2C();
        this->begin(devId);
    }

    void beginTransmission(const char* device, int devId) {
        this->lockI2C();
        this->begin(device, devId);
    }

    uint8_t endTransmission() {
        uint8_t status {0};
        status = this->endWriteData();
        this->unlockI2C();
        return status;
    }

    size_t requestFrom(const int devId, size_t size) {
        if (!this->connected()) {
            return 0;
        }
        if (this->rxBuffer == NULL) {
            return 0;
        }
        if (devId != this->txAddress) {
            return 0;
        }
        if (size > this->bufferSize) {
            size = this->bufferSize;
        }
        this->lockI2C();
        this->rxIndex = 0;
        this->rxLength = this->readData(this->rxBuffer, size);
        this->unlockI2C();
        return this->rxLength;
    }

protected:
#if defined(WIRING_PI)
    int setup(const int devId) {
        return wiringPiI2CSetup(devId);
    }

    int setupInterface(const char* device, int devId) {
        return wiringPiI2CSetupInterface(device, devId);
    }
#else
    int setup(const int devId) {
        return this->setupInterface("/dev/i2c-1", devId);
    }

    int setupInterface(const char* device, int devId) {
        int fd;

        fd = ::open(device, O_RDWR);
        if (fd < 0) {
            return -1;
        }
        if (ioctl(fd, I2C_SLAVE, devId) < 0) {
            ::close(fd);
            return -1;
        }

        return fd;
    }
#endif

#if defined(ERA_I2C_DEV)
    uint8_t endWriteData() {
        if (!this->connected()) {
            return I2CStatusT::I2C_STATUS_NOT_CONNECT;
        }
        if (this->txBuffer == NULL) {
            return I2CStatusT::I2C_STATUS_NULL_BUFFER;
        }
        int bytes = ::write(this->fd, this->txBuffer, this->txLength);
        if ((bytes < 0) && ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))) {
            return I2CStatusT::I2C_STATUS_SUCCESS;
        }
        if (bytes != (int)this->txLength) {
            return I2CStatusT::I2C_STATUS_FAILED_WRITE;
        }
        return I2CStatusT::I2C_STATUS_SUCCESS;
    }

    size_t readData(uint8_t* buffer, size_t size) {
        int bytes = ::read(this->fd, this->rxBuffer, size);
        if ((bytes < 0) && ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))) {
            return 0;
        }
        else if (bytes <= 0) {
            return 0;
        }
        return (size_t)bytes;
    }
#else
    uint8_t endWriteData() {
        if (!this->connected()) {
            return I2CStatusT::I2C_STATUS_NOT_CONNECT;
        }
        if (this->txBuffer == NULL) {
            return I2CStatusT::I2C_STATUS_NULL_BUFFER;
        }
        size_t bytes {0};
        for (size_t i = 0; i < this->txLength; ++i) {
            if (!wiringPiI2CWrite(this->fd, this->txBuffer[i])) {
                bytes++;
            }
        }
        if (bytes != this->txLength) {
            return I2CStatusT::I2C_STATUS_FAILED_WRITE;
        }
        return I2CStatusT::I2C_STATUS_SUCCESS;
    }

    int readByte() {
        if (!this->connected()) {
            return -1;
        }
        int c {0};
        this->startMillis = this->unix_millis();
        do {
            c = wiringPiI2CRead(this->fd);
            if (c >= 0) {
                return c;
            }
        } while ((this->unix_millis() - this->startMillis) < this->timeout);
        return -1;
    }

    size_t readData(uint8_t* buffer, size_t size) {
        if (buffer == NULL) {
            return 0;
        }
        size_t count {0};
        while (count < size) {
            int c = this->readByte();
            if (c < 0) {
                break;
            }
            *buffer++ = (uint8_t)c;
            count++;
        }
        return count;
    }
#endif

private:
    bool connected() {
        return (this->fd >= 0);
    }

    void lockI2C() {
        if (this->disableMutex) {
            return;
        }
        ERaGuardLock(this->mutex);
    }

    void unlockI2C() {
        if (this->disableMutex) {
            return;
        }
        ERaGuardUnlock(this->mutex);
    }

    int fd;

    size_t bufferSize;
    uint8_t* rxBuffer;
    size_t rxIndex;
    size_t rxLength;

    uint8_t* txBuffer;
    size_t txLength;
    uint16_t txAddress;

    bool disableMutex;
    ERaMutex_t mutex;
};

typedef ERaI2CWiringPi ERaI2C;
typedef ERaI2CWiringPi TwoWire;

#endif /* INC_ERA_I2C_WIRING_PI_HPP_ */
