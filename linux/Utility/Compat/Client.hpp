#ifndef INC_CLIENT_HPP_
#define INC_CLIENT_HPP_

#include "Stream.hpp"
#include "IPAddress.hpp"

class Client
    : public Stream
{
public:
    virtual int connect(IPAddress ip, uint16_t port) = 0;
    virtual int connect(const char* host, uint16_t port) = 0;
    virtual size_t write(uint8_t value) = 0;
    virtual size_t write(const uint8_t* buf, size_t size) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int read(uint8_t* buf, size_t size) = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    virtual void stop() = 0;
    virtual uint8_t connected() = 0;
    virtual operator bool() = 0;

protected:
    uint8_t* rawIPAddress(IPAddress& addr) {
        return addr.rawAddress();
    }

};

#endif /* INC_CLIENT_HPP_ */
