#ifndef INC_ERA_SOCKET_LINUX_HPP_
#define INC_ERA_SOCKET_LINUX_HPP_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Compat/Client.hpp"

class ERaSocketLinux
    : public Client
{
public:
    ERaSocketLinux()
        : fd(-1)
        , timeout(1000L)
    {}
    ~ERaSocketLinux()
    {}

    int connect(IPAddress ip, uint16_t port) {
        this->disconnect();

        // populate address struct
        uint32_t ip_addr = ip;
        struct sockaddr_in address;
        address.sin_port = htons(port);
        address.sin_family = AF_INET;
        memcpy((void*)&address.sin_addr.s_addr, (const void*)(&ip_addr), 4);

        // create new socket
        this->fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->fd < 0) {
            return 0;
        }

        // connect socket
        int rc = ::connect(this->fd, (struct sockaddr*)&address, sizeof(address));
        if (rc < 0) {
            return 0;
        }

        return 1;
    }

    int connect(const char* host, uint16_t port) {
        this->disconnect();

        // prepare resolver hints
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_flags = AI_ADDRCONFIG;
        hints.ai_socktype = SOCK_STREAM;

        // resolve address
        struct addrinfo* result = NULL;
        int rc = getaddrinfo(host, NULL, &hints, &result);
        if (rc < 0) {
            return 0;
        }

        // prepare selected result
        struct addrinfo* current = result;
        struct addrinfo* selected = NULL;

        // traverse list and select first found ipv4 address
        while (current) {
            // check if ipv4 address
            if (current->ai_family == AF_INET) {
                selected = current;
                break;
            }

            // move one to next
            current = current->ai_next;
        }

        // return error if none found
        if (selected == NULL) {
            return 0;
        }

        // populate address struct
        struct sockaddr_in address;
        address.sin_port = htons(port);
        address.sin_family = AF_INET;
        address.sin_addr = ((struct sockaddr_in*)(selected->ai_addr))->sin_addr;

        // free result
        freeaddrinfo(result);

        // create new socket
        this->fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->fd < 0) {
            return 0;
        }

        // connect socket
        rc = ::connect(this->fd, (struct sockaddr*)&address, sizeof(address));
        if (rc < 0) {
            return 0;
        }

        return 1;
    }

    int select() {
        // prepare set
        fd_set set;
        fd_set ex_set;
        FD_ZERO(&set);
        FD_ZERO(&ex_set);
        FD_SET(this->fd, &set);
        FD_SET(this->fd, &ex_set);

        // wait for data
        struct timeval t = {
            .tv_sec = this->timeout / 1000,
            .tv_usec = (this->timeout % 1000) * 1000
        };
        int result = ::select(this->fd + 1, &set, NULL, &ex_set, &t);
        if ((result < 0) || FD_ISSET(this->fd, &ex_set)) {
            return -1;
        }

        return 1;
    }

    void disconnect() {
        this->stop();
    }

    size_t write(uint8_t value) {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buf, size_t size) {
        struct timeval t = {
            .tv_sec = this->timeout / 1000,
            .tv_usec = (this->timeout % 1000) * 1000
        };
        int rc = setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, sizeof(t));
        if (rc < 0) {
            return 0;
        }

        // write to socket
        int bytes = (int)::send(this->fd, buf, size, 0);
        if ((bytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            return 0;
        } else if (bytes < 0) {
            return 0;
        }

        return bytes;
    }

    int available() {
        if (!this->connected()) {
            return 0;
        }

        int count {0};
        if (ioctl(this->fd, FIONREAD, &count) == 0) {
            if (!count) {
                usleep(10000);
            }
            return count;
        }
        return 0;
    }

    int read() {
        uint8_t ret {0};

        if (this->read(&ret, 1) < 0) {
            return -1;
        }

        return (int)ret;
    }

    int read(uint8_t* buf, size_t size) {
        struct timeval t = {
            .tv_sec = this->timeout / 1000,
            .tv_usec = (this->timeout % 1000) * 1000
        };
        int rc = setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
        if (rc < 0) {
            return -1;
        }

        // read from socket
        int bytes = (int)::read(this->fd, buf, size);
        if ((bytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            return -1;
        } else if (bytes == 0) {
            return -1;
        } else if (bytes < 0) {
            return -1;
        }

        return bytes;
    }

    int peek() {
        uint8_t ret {0};

        int bytes = (int)::recv(this->fd, &ret, 1, MSG_PEEK | MSG_DONTWAIT);
        if ((bytes <= 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
            return -1;
        }

        return (int)ret;
    }

    void flush() {
        int bytes {0};
        size_t toRead {0};
        size_t a = this->available();
        if (!a) {
            return;
        }
        uint8_t* buf = (uint8_t*)malloc(1024);
        if (buf == NULL) {
            return;
        }
        while (a) {
            toRead = ((a > 1024) ? 1024 : a);
            bytes = (int)::recv(this->fd, buf, toRead, MSG_DONTWAIT);
            if(bytes < 0) {
                break;
            }
            a -= bytes;
        }
        free(buf);
        buf = NULL;
    }

    void stop() {
        if (this->fd == -1) {
            return;
        }
        while (::close(this->fd) < 0) {
            usleep(10000);
        }
        this->fd = -1;
    }

    uint8_t connected() {
        return (this->fd >= 0);
    }

    operator bool() {
        return this->connected();
    }

private:
    int fd;
    unsigned long timeout;

};

#endif /* INC_ERA_SOCKET_LINUX_HPP_ */
