#ifndef INC_ERA_UDP_LINUX_HPP_
#define INC_ERA_UDP_LINUX_HPP_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERaUtility.hpp>
#include <Utility/Compat/Udp.hpp>

class ERaUDPLinux
    : public UDP
{
public:
    ERaUDPLinux()
        : udpServer(-1)
        , serverPort(0)
        , _remotePort(0)
        , txBuffer(NULL)
        , txBufferLen(0)
        , timeout(1000L)
    {}
    ~ERaUDPLinux()
    {
        this->stop();
    }

    uint8_t begin(IPAddress address, uint16_t port) {
        this->stop();

        this->serverPort = port;

        this->txBuffer = (uint8_t*)malloc(ERA_MAX_WRITE_BYTES * 2);
        if (this->txBuffer == NULL) {
            return 0;
        }

        this->udpServer = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (this->udpServer < 0) {
            return 0;
        }

        int yes = 1;
        int rc = setsockopt(this->udpServer, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (rc < 0) {
            this->stop();
            return 0;
        }

        struct sockaddr_in addr;
        memset((char*)&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(this->serverPort);
        addr.sin_addr.s_addr = (in_addr_t)address;
        rc = ::bind(this->udpServer, (struct sockaddr*)&addr, sizeof(addr));
        if (rc < 0) {
            this->stop();
            return 0;
        }

        fcntl(this->udpServer, F_SETFL, O_NONBLOCK);
        return 1;
    }

    uint8_t begin(uint16_t port) override {
        return this->begin(IPAddress(INADDR_ANY), port);
    }

    uint8_t beginMulticast(IPAddress address, uint16_t port) override {
        if (this->begin(port)) {
            if (address != 0) {
                struct ip_mreq mreq;
                mreq.imr_multiaddr.s_addr = (in_addr_t)address;
                mreq.imr_interface.s_addr = INADDR_ANY;
                if (setsockopt(this->udpServer, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
                    this->stop();
                    return 0;
                }
                this->multicastIP = address;
            }
            return 1;
        }
        return 0;
    }

    void stop() override {
        if (this->txBuffer != NULL) {
            free(this->txBuffer);
            this->txBuffer = NULL;
        }
        this->txBufferLen = 0;
        this->rxBuffer.clear();
        if (this->udpServer < 0) {
            return;
        }
        if (this->multicastIP != 0) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = (in_addr_t)this->multicastIP;
            mreq.imr_interface.s_addr = (in_addr_t)0;
            setsockopt(this->udpServer, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
            this->multicastIP = IPAddress(INADDR_ANY);
        }
        ::close(this->udpServer);
        this->udpServer = -1;
    }

    int beginMulticastPacket() {
        if (!this->serverPort ||
            (this->multicastIP == IPAddress(INADDR_ANY))) {
            return 0;
        }
        this->_remoteIP = this->multicastIP;
        this->_remotePort = this->serverPort;
        return this->beginPacket();
    }

    int beginPacket() {
        if (!this->_remotePort) {
            return 0;
        }

        if (this->txBuffer == NULL) {
            this->txBuffer = (uint8_t*)malloc(ERA_MAX_WRITE_BYTES * 2);
            if (this->txBuffer == NULL) {
                return 0;
            }
        }

        this->txBufferLen = 0;

        if (this->udpServer >= 0) {
            return 1;
        }

        this->udpServer = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (this->udpServer < 0) {
            return 0;
        }

        fcntl(this->udpServer, F_SETFL, O_NONBLOCK);
        return 1;
    }

    int beginPacket(IPAddress ip, uint16_t port) override {
        this->_remoteIP = ip;
        this->_remotePort = port;
        return this->beginPacket();
    }

    int beginPacket(const char* host, uint16_t port) override {
        struct hostent* server;
        server = gethostbyname(host);
        if (server == NULL) {
            return 0;
        }
        return this->beginPacket(IPAddress((const uint8_t*)(server->h_addr_list[0])), port);
    }

    int endPacket() override {
        struct sockaddr_in recipient;
        recipient.sin_addr.s_addr = (uint32_t)this->_remoteIP;
        recipient.sin_family = AF_INET;
        recipient.sin_port = htons(this->_remotePort);

        int sent = sendto(this->udpServer, this->txBuffer, this->txBufferLen, 0, (struct sockaddr*)&recipient, sizeof(recipient));
        if (sent < 0) {
            return 0;
        }
        return 1;
    }

    size_t write(uint8_t value) override {
        if (this->txBufferLen == ERA_MAX_WRITE_BYTES * 2) {
            this->endPacket();
            this->txBufferLen = 0;
        }
        this->txBuffer[this->txBufferLen++] = value;
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        for (size_t i = 0; i < size; ++i) {
            this->write(buffer[i]);
        }
        return size;
    }

    int parsePacket() override {
        if (!this->rxBuffer.space()) {
            return 0;
        }

        struct sockaddr_in si_other;
        int slen = sizeof(si_other);
        int len {0};
        uint8_t* buf = (uint8_t*)malloc(ERA_MAX_WRITE_BYTES * 2);
        if (buf == NULL) {
            return 0;
        }

        len = recvfrom(this->udpServer, buf, ERA_MAX_WRITE_BYTES * 2, MSG_DONTWAIT, (struct sockaddr*)&si_other, (socklen_t*)&slen);
        if (len < 0) {
            free(buf);
            buf = NULL;
            if (errno == EWOULDBLOCK) {
                return 0;
            }
            return 0;
        }

        this->_remoteIP = IPAddress(si_other.sin_addr.s_addr);
        this->_remotePort = ntohs(si_other.sin_port);

        if (len > 0) {
            this->rxBuffer.put(buf, len);
        }

        free(buf);
        buf = NULL;
        return len;
    }

    int available() override {
        return this->rxBuffer.size();
    }

    int read() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.get();
    }

    int read(unsigned char* buffer, size_t len) override {
        MillisTime_t startMillis = ERaMillis();
        while (ERaMillis() - startMillis < this->timeout) {
            if (this->available() < (int)len) {
                ERaDelay(1);
            }
            else {
                break;
            }
        }
        return this->rxBuffer.get(buffer, len);
    }

    int read(char* buffer, size_t len) override {
        return this->read((uint8_t*)buffer, len);
    }

    int peek() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.peek();
    }

    void flush() override {
        this->rxBuffer.clear();
    }

    IPAddress remoteIP() override {
        return this->_remoteIP;
    }

    uint16_t remotePort() override {
        return this->_remotePort;
    }

private:
    int udpServer;
    IPAddress multicastIP;
    IPAddress _remoteIP;
    uint16_t serverPort;
    uint16_t _remotePort;
    uint8_t* txBuffer;
    size_t txBufferLen;
    ERaQueue<uint8_t, ERA_MAX_READ_BYTES * 2> rxBuffer;
    unsigned long timeout;
};

typedef ERaUDPLinux LinuxUDP;

#endif /* INC_ERA_UDP_LINUX_HPP_ */
