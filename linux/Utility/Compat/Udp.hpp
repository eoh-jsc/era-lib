#ifndef INC_UDP_HPP_
#define INC_UDP_HPP_

#include "Stream.hpp"
#include "IPAddress.hpp"

class UDP
    : public Stream
{

public:
    virtual uint8_t begin(uint16_t port) = 0;
    virtual uint8_t beginMulticast(IPAddress address, uint16_t port) { return 0; }
    virtual void stop() = 0;
    virtual int beginPacket(IPAddress ip, uint16_t port) = 0;
    virtual int beginPacket(const char* host, uint16_t port) = 0;
    virtual int endPacket() = 0;
    virtual size_t write(uint8_t value) = 0;
    virtual size_t write(const uint8_t* buffer, size_t size) = 0;
    virtual int parsePacket() = 0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int read(unsigned char* buffer, size_t len) = 0;
    virtual int read(char* buffer, size_t len) = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    virtual IPAddress remoteIP() = 0;
    virtual uint16_t remotePort() = 0;
protected:
    uint8_t* rawIPAddress(IPAddress& addr) {
        return addr.rawAddress();
    }

};

#endif /* INC_UDP_HPP_ */
