#ifndef INC_IPADDRESS_HPP_
#define INC_IPADDRESS_HPP_

#include <stdint.h>

class Client;

class IPAddress
{
    union {
        uint8_t bytes[4];  // IPv4 address
        uint32_t dword;
    } address;

public:
    IPAddress() {
        this->address.dword = 0;
    }

    IPAddress(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
        this->address.bytes[0] = first;
        this->address.bytes[1] = second;
        this->address.bytes[2] = third;
        this->address.bytes[3] = fourth;
    }

    IPAddress(uint32_t _address) {
        this->address.dword = _address;
    }

    IPAddress(const uint8_t *_address) {
        memcpy(this->address.bytes, _address, sizeof(this->address.bytes));
    }

    virtual ~IPAddress()
    {}

    operator uint32_t() const {
        return this->address.dword;
    }

    bool operator==(const IPAddress& addr) const {
        return (this->address.dword == addr.address.dword);
    }

    bool operator==(const uint8_t* addr) const {
        return !memcmp(addr, this->address.bytes, sizeof(this->address.bytes));
    }

    uint8_t operator[](int index) const {
        return this->address.bytes[index];
    }

    uint8_t& operator[](int index) {
        return this->address.bytes[index];
    }

    IPAddress& operator=(const uint8_t *_address) {
        memcpy(this->address.bytes, _address, sizeof(this->address.bytes));
        return *this;
    }

    IPAddress& operator=(uint32_t _address) {
        this->address.dword = _address;
        return *this;
    }

private:
    uint8_t* rawAddress() {
        return this->address.bytes;
    }

    friend class Client;

};

#endif /* INC_IPADDRESS_HPP_ */
