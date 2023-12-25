#ifndef INC_ERA_SOCKET_SECURE_LINUX_HPP_
#define INC_ERA_SOCKET_SECURE_LINUX_HPP_

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>
#include <Network/ERaSocketLinux.hpp>

class ERaSocketSecureLinux
    : public ERaSocketLinux
{
    typedef struct __TLSNetwork_t {
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_ssl_context ssl;
        mbedtls_ssl_config conf;
        mbedtls_x509_crt cacert;
        mbedtls_net_context socket;
        uint8_t* ca_buf;
        size_t ca_len;
        bool verify;
    } TLSNetwork_t;

public:
    ERaSocketSecureLinux()
        : network{}
        , timeout(1000L)
        , _connected(false)
    {
        this->begin(false, nullptr, 0);
    }
    ~ERaSocketSecureLinux()
    {
        this->disconnect();
    }

    void begin(bool verify, const uint8_t* caBuf, size_t caLen) {
        this->network.verify = verify;
        this->network.ca_buf = (uint8_t*)caBuf;
        this->network.ca_len = caLen;
    }

    int connect(IPAddress ip, uint16_t port) override {
        char host[20] {0};
        snprintf(host, sizeof(host), "%d.%d.%d.%d",
                ip[0], ip[1], ip[2], ip[3]);
        return this->connect(host, port);
    }

    int connect(const char* host, uint16_t port) override {
        this->disconnect();

        static char port_string[10] {0};
        memset(port_string, 0, sizeof(port_string));
        snprintf(port_string, sizeof(port_string), "%d", port);

        // initialize support structures
        mbedtls_net_init(&this->network.socket);
        mbedtls_ssl_init(&this->network.ssl);
        mbedtls_ssl_config_init(&this->network.conf);
        mbedtls_x509_crt_init(&this->network.cacert);
        mbedtls_ctr_drbg_init(&this->network.ctr_drbg);
        mbedtls_entropy_init(&this->network.entropy);

        // setup entropy source
        int ret = mbedtls_ctr_drbg_seed(&this->network.ctr_drbg, mbedtls_entropy_func, &this->network.entropy, NULL, 0);
        if (ret != 0) {
            return 0;
        }

        // parse ca certificate
        if (this->network.ca_buf) {
            ret = mbedtls_x509_crt_parse(&this->network.cacert, this->network.ca_buf, this->network.ca_len);
            if (ret != 0) {
                return 0;
            }
        }

        // connect socket
        ret = mbedtls_net_connect(&this->network.socket, host, port_string, MBEDTLS_NET_PROTO_TCP);
        if (ret != 0) {
            return 0;
        }

        // load defaults
        ret = mbedtls_ssl_config_defaults(&this->network.conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT);
        if (ret != 0) {
            return 0;
        }

        // set ca certificate
        if (this->network.ca_buf) {
            mbedtls_ssl_conf_ca_chain(&this->network.conf, &this->network.cacert, NULL);
        }

        // set auth mode
        mbedtls_ssl_conf_authmode(&this->network.conf, (this->network.verify) ? MBEDTLS_SSL_VERIFY_REQUIRED : MBEDTLS_SSL_VERIFY_NONE);

        // set rng callback
        mbedtls_ssl_conf_rng(&this->network.conf, mbedtls_ctr_drbg_random, &this->network.ctr_drbg);

        // setup ssl context
        ret = mbedtls_ssl_setup(&this->network.ssl, &this->network.conf);
        if (ret != 0) {
            return 0;
        }

        // set hostname
        ret = mbedtls_ssl_set_hostname(&this->network.ssl, host);
        if (ret != 0) {
            return 0;
        }

        // set bio callbacks
        mbedtls_ssl_set_bio(&this->network.ssl, &this->network.socket, mbedtls_net_send, mbedtls_net_recv, NULL);

        // verify certificate if requested
        if (this->network.verify) {
            uint32_t flags = mbedtls_ssl_get_verify_result(&this->network.ssl);
            if (flags != 0) {
                char verify_buf[100] = {0};
                mbedtls_x509_crt_verify_info(verify_buf, sizeof(verify_buf), "  ! ", flags);
            }
        }

        // perform handshake
        ret = mbedtls_ssl_handshake(&this->network.ssl);
        if (ret != 0) {
            return 0;
        }

        int rc = this->networkWait(this->timeout);
        if (rc <= 0) {
            return 0;
        }

        this->_connected = true;
        return 1;
    }

    void disconnect() {
        this->stop();
    }

    void setTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

    unsigned long getTimeout() const {
        return this->timeout;
    }

    int networkWait(unsigned long _timeout) {
        // prepare sets
        fd_set set;
        fd_set ex_set;
        FD_ZERO(&set);
        FD_ZERO(&ex_set);
        FD_SET(this->network.socket.fd, &set);
        FD_SET(this->network.socket.fd, &ex_set);

        // wait for data
        struct timeval t = {.tv_sec = this->timeout / 1000, .tv_usec = (this->timeout % 1000) * 1000};
        int result = select(this->network.socket.fd + 1, NULL, &set, &ex_set, &t);
        if ((result < 0) || FD_ISSET(this->network.socket.fd, &ex_set)) {
            return result;
        }

        // set socket to blocking
        int ret = mbedtls_net_set_block(&this->network.socket);
        if (ret < 0) {
            return ret;
        }

        return result;
    }

    size_t write(uint8_t value) override {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buf, size_t size) override {
        if (!this->_connected) {
            return 0;
        }

        // TODO Block until ready for writing
        int rc = this->networkWait(this->timeout);
        if (rc < 0) {
            this->close();
            return 0;
        }
        else if (rc == 0) {
            return 0;
        }

        // set timeout
        struct timeval t = {.tv_sec = this->timeout / 1000, .tv_usec = (this->timeout % 1000) * 1000};
        rc = setsockopt(this->network.socket.fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&t, sizeof(t));
        if (rc < 0) {
            this->close();
            return 0;
        }

        // write to socket
        int ret = mbedtls_ssl_write(&this->network.ssl, buf, size);
        if ((ret == MBEDTLS_ERR_SSL_WANT_READ) ||
            (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return 0;
        } else if (ret < 0) {
            this->close();
            return 0;
        }

        return ret;
    }

    int available() override {
        if (!this->_connected) {
            return 0;
        }

        // prepare set
        fd_set set;
        fd_set ex_set;
        FD_ZERO(&set);
        FD_ZERO(&ex_set);
        FD_SET(this->network.socket.fd, &set);
        FD_SET(this->network.socket.fd, &ex_set);

        // wait for data
        struct timeval t = {.tv_sec = this->timeout / 1000, .tv_usec = (this->timeout % 1000) * 1000};
        int result = select(this->network.socket.fd + 1, &set, NULL, &ex_set, &t);
        if ((result < 0) || FD_ISSET(this->network.socket.fd, &ex_set)) {
            this->close();
            return 0;
        }

        if (result <= 0) {
            return 0;
        }

        int rc = setsockopt(this->network.socket.fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
        if (rc < 0) {
            this->close();
            return 0;
        }

        // set socket to non blocking
        int ret = mbedtls_net_set_nonblock(&this->network.socket);
        if (ret != 0) {
            this->close();
            return 0;
        }

        // check if socket is valid
        ret = mbedtls_ssl_read(&this->network.ssl, NULL, 0);
        if ((ret < 0) && (ret != MBEDTLS_ERR_SSL_WANT_READ) &&
                        (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            this->close();
            return 0;
        }

        // set available bytes
        size_t available = mbedtls_ssl_get_bytes_avail(&this->network.ssl);

        // set socket back to blocking
        ret = mbedtls_net_set_block(&this->network.socket);
        if (ret != 0) {
            this->close();
            return 0;
        }

        return (int)available;
    }

    int read() override {
        uint8_t ret {0};

        int rc = this->read(&ret, 1);
        if (rc < 0) {
            return rc;
        }
        else if (rc == 0) { // No data available.
            return -1;
        }

        return (int)ret;
    }

    int read(uint8_t* buf, size_t size) override {
        if (!this->available()) {
            return 0;
        }

        // set timeout
        struct timeval t = {.tv_sec = this->timeout / 1000, .tv_usec = (this->timeout % 1000) * 1000};
        int rc = setsockopt(this->network.socket.fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
        if (rc < 0) {
            this->close();
            return rc;
        }

        // read from socket
        int ret = mbedtls_ssl_read(&this->network.ssl, buf, size);
        if ((ret == MBEDTLS_ERR_SSL_WANT_READ) ||
            (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return 0;
        } else if (ret <= 0) {
            this->close();
            return ret;
        }

        return ret;
    }

    int peek() override {
        return -1;
    }

    void flush() override {
    }

    void stop() override {
        this->_connected = false;
        if (this->network.socket.fd < 0) {
            return;
        }

        // cleanup resources
        int ret;
        do {
            ret = mbedtls_ssl_close_notify(&this->network.ssl);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

        mbedtls_x509_crt_free(&this->network.cacert);
        mbedtls_entropy_free(&this->network.entropy);
        mbedtls_ssl_config_free(&this->network.conf);
        mbedtls_ctr_drbg_free(&this->network.ctr_drbg);
        mbedtls_ssl_free(&this->network.ssl);
        mbedtls_net_free(&this->network.socket);

        this->network.socket.fd = -1;
    }

    void close() {
        this->_connected = false;
    }

    uint8_t connected() override {
        return this->_connected;
    }

    operator bool() override {
        return this->connected();
    }

private:
    TLSNetwork_t network;

    unsigned long timeout;
    bool _connected;
};

typedef ERaSocketSecureLinux ERaSocketSecure;

#endif /* INC_ERA_SOCKET_SECURE_LINUX_HPP_ */
