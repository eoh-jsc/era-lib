/**
 * @file       TinyGsmTCPUDP.tpp
 * Add UDP for ESP32
 */

#ifndef SRC_TINYGSM_TCP_UDP_HPP_
#define SRC_TINYGSM_TCP_UDP_HPP_

#include <TinyGsmCommon.h>

#define TINY_GSM_MODEM_HAS_TCP
#define TINY_GSM_MODEM_HAS_UDP

#include "TinyGsmFifo.hpp"

#if !defined(TINY_GSM_RX_BUFFER)
    #define TINY_GSM_RX_BUFFER 64
#endif

// Because of the ordering of resolution of overrides in templates, these need
// to be written out every time.  This macro is to shorten that.
#define TINY_GSM_CLIENT_CONNECT_OVERRIDES                                   \
    int connect(IPAddress ip, uint16_t port, int timeout_s) {               \
        return connect(TinyGsmStringFromIp(ip).c_str(), port, timeout_s);   \
    }                                                                       \
    int connect(const char* host, uint16_t port) override {                 \
        return connect(host, port, 75);                                     \
    }                                                                       \
    int connect(IPAddress ip, uint16_t port) override {                     \
        return connect(ip, port, 75);                                       \
    }

#define TINY_GSM_UDP_BEGIN_OVERRIDES                                        \
    uint8_t begin(uint16_t port) override {                                 \
        return begin(IPAddress(0, 0, 0, 0), port);                          \
    }                                                                       \
    uint8_t begin(IPAddress address, uint16_t port) {                       \
        return begin(address, port, 75);                                    \
    }                                                                       \
    uint8_t begin(IPAddress address, uint16_t port, int timeout_s) {        \
        return begin(TinyGsmStringFromIp(address).c_str(), port, timeout_s);\
    }                                                                       \
    uint8_t beginMulticast(IPAddress address, uint16_t port) override {     \
        return begin(address, port, 75);                                    \
    }

// // For modules that do not store incoming data in any sort of buffer
// #define TINY_GSM_NO_MODEM_BUFFER
// // Data is stored in a buffer, but we can only read from the buffer,
// // not check how much data is stored in it
// #define TINY_GSM_BUFFER_READ_NO_CHECK
// // Data is stored in a buffer and we can both read and check the size
// // of the buffer
// #define TINY_GSM_BUFFER_READ_AND_CHECK_SIZE

template <class modemType, uint8_t muxCount>
class TinyGsmTCP {
public:
    /*
    * Basic functions
    */
    void maintain() {
        return this->thisModem().maintainImpl();
    }

    /*
    * CRTP Helper
    */
    protected:
    inline const modemType& thisModem() const {
        return static_cast<const modemType&>(*this);
    }
    inline modemType& thisModem() {
        return static_cast<modemType&>(*this);
    }

    /*
    * Inner Client, UDP
    */
public:
    class GsmClient 
        : public Client
        , public UDP
    {
        // Make all classes created from the modem template friends
        friend class TinyGsmTCP<modemType, muxCount>;
        typedef TinyGsmFifo<uint8_t, TINY_GSM_RX_BUFFER> RxFifo;

    public:
        inline
        static String TinyGsmStringFromIp(IPAddress ip) {
            String host;
            host.reserve(16);
            host += ip[0];
            host += ".";
            host += ip[1];
            host += ".";
            host += ip[2];
            host += ".";
            host += ip[3];
            return host;
        }

        inline
        static bool TinyGsmIpFromString(IPAddress& ip, const char* address) {
            uint16_t acc = 0; // Accumulator
            uint8_t dots = 0;

            while (*address) {
                char c = *address++;
                if ((c >= '0') && (c <= '9')) {
                    acc = (acc * 10) + (c - '0');
                    if (acc > 255) {
                        // Value out of [0..255] range
                        return false;
                    }
                }
                else if (c == '.') {
                    if (dots == 3) {
                        // Too much dots (there must be 3 dots)
                        return false;
                    }
                    ip[dots++] = acc;
                    acc = 0;
                }
                else if (c == '"') {
                    continue;
                }
                else {
                    // Invalid char
                    return false;
                }
            }

            if (dots != 3) {
                // Too few dots (there must be 3 dots)
                return false;
            }
            ip[3] = acc;
            return true;
        }

        // Writes data out on the client using the modem send functionality
        size_t write(const uint8_t* buf, size_t size) override {
            TINY_GSM_YIELD();
            this->at->maintain();
            if (!this->isUDP) {
                return this->at->modemSend(buf, size, mux);
            }
            else {
                return this->at->modemSendUDP(buf, size, mux);
            }
        }

        size_t write(uint8_t c) override {
            return this->write(&c, 1);
        }

        size_t write(const char* str) {
            if (str == NULL) {
                return 0;
            }
            return this->write((const uint8_t*)str, strlen(str));
        }

        int available() override {
            TINY_GSM_YIELD();
#if defined(TINY_GSM_NO_MODEM_BUFFER)
            // Returns the number of characters available in the TinyGSM fifo
            if (!this->rx.size() && this->sock_connected) {
                this->at->maintain();
            }
            return this->rx.size();

#elif defined(TINY_GSM_BUFFER_READ_NO_CHECK)
            // Returns the combined number of characters available in the TinyGSM
            // fifo and the modem chips internal fifo.
            if (!this->rx.size()) {
                this->at->maintain();
            }
            return (static_cast<uint16_t>(this->rx.size()) + this->sock_available);

#elif defined(TINY_GSM_BUFFER_READ_AND_CHECK_SIZE)
            // Returns the combined number of characters available in the TinyGSM
            // fifo and the modem chips internal fifo, doing an extra check-in
            // with the modem to see if anything has arrived without a UURC.
            if (!this->rx.size()) {
                if ((millis() - this->prev_check) > 500) {
                    // setting got_data to true will tell maintain to run
                    // modemGetAvailable(mux)
                    this->got_data   = true;
                    this->prev_check = millis();
                }
                this->at->maintain();
            }
            return (static_cast<uint16_t>(this->rx.size()) + this->sock_available);

#else
            #error "Modem client has been incorrectly created"
#endif
        }

        int read(unsigned char* buf, size_t size) override {
            TINY_GSM_YIELD();
            size_t cnt = 0;

#if defined(TINY_GSM_NO_MODEM_BUFFER)
            // Reads characters out of the TinyGSM fifo, waiting for any URC's
            // from the modem for new data if there's nothing in the fifo.
            uint32_t _startMillis = millis();
            while ((cnt < size) && ((millis() - _startMillis) < Client::_timeout)) {
                size_t chunk = TinyGsmMin(size - cnt, this->rx.size());
                if (chunk > 0) {
                    this->rx.get(buf, chunk);
                    buf += chunk;
                    cnt += chunk;
                    continue;
                } /* TODO: Read directly into user buffer? */
                if (!this->rx.size() && this->sock_connected) {
                    this->at->maintain();
                }
            }
            return cnt;

#elif defined(TINY_GSM_BUFFER_READ_NO_CHECK)
            // Reads characters out of the TinyGSM fifo, and from the modem chip's
            // internal fifo if available.
            this->at->maintain();
            while (cnt < size) {
                size_t chunk = TinyGsmMin(size - cnt, this->rx.size());
                if (chunk > 0) {
                    this->rx.get(buf, chunk);
                    buf += chunk;
                    cnt += chunk;
                    continue;
                }
                /* TODO: Read directly into user buffer? */
                this->at->maintain();
                if (this->sock_available > 0) {
                    int n = this->at->modemRead(TinyGsmMin((uint16_t)this->rx.free(),
                                                this->sock_available), this->mux);
                    if (n == 0) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
            return cnt;

#elif defined(TINY_GSM_BUFFER_READ_AND_CHECK_SIZE)
            // Reads characters out of the TinyGSM fifo, and from the modem chips
            // internal fifo if available, also double checking with the modem if
            // data has arrived without issuing a UURC.
            this->at->maintain();
            while (cnt < size) {
                size_t chunk = TinyGsmMin(size - cnt, this->rx.size());
                if (chunk > 0) {
                    this->rx.get(buf, chunk);
                    buf += chunk;
                    cnt += chunk;
                    continue;
                }
                // Workaround: Some modules "forget" to notify about data arrival
                if ((millis() - this->prev_check) > 500) {
                    // setting got_data to true will tell maintain to run
                    // modemGetAvailable()
                    this->got_data   = true;
                    this->prev_check = millis();
                }
                this->at->maintain();
                if (this->sock_available > 0) {
                    int n = this->at->modemRead(TinyGsmMin((uint16_t)this->rx.free(),
                                                this->sock_available), this->mux);
                    if (n == 0) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
            return cnt;

#else
            #error "Modem client has been incorrectly created"
#endif
        }

        int read() override {
            uint8_t c {0};
            if (this->read(&c, 1) == 1) {
                return c;
            }
            return -1;
        }

        int read(char* buf, size_t size) override {
            return this->read((uint8_t*)buf, size);
        }

        int peek() override {
            return this->rx.peek();
        }

        void flush() override {
            this->at->stream.flush();
        }

        uint8_t connected() override {
            if (this->available()) {
                return true;
            }
#if defined(TINY_GSM_BUFFER_READ_AND_CHECK_SIZE)
            // If the modem is one where we can read and check the size of the buffer,
            // then the 'available()' function will call a check of the current size
            // of the buffer and state of the connection. [available calls maintain,
            // maintain calls modemGetAvailable, modemGetAvailable calls
            // modemGetConnected]  This cascade means that the sock_connected value
            // should be correct and all we need
            return this->sock_connected;
#elif defined(TINY_GSM_NO_MODEM_BUFFER) || defined(TINY_GSM_BUFFER_READ_NO_CHECK)
            // If the modem doesn't have an internal buffer, or if we can't check how
            // many characters are in the buffer then the cascade won't happen.
            // We need to call modemGetConnected to check the sock state.
            return this->at->modemGetConnected(mux);
#else
            #error "Modem client has been incorrectly created"
#endif
        }

        operator bool() {
            return this->connected();
        }

        int beginPacket() {
            //TODO: Update later
            return 1;
        }

        int beginPacket(IPAddress ip, uint16_t port) override {
            this->_remoteIP = ip;
            this->_remotePort = port;
            return 1;
        }

        int beginPacket(const char* host, uint16_t port) override {
            TinyGsmIpFromString(this->_remoteIP, host);
            this->_remotePort = port;
            return 1;
        }

        int endPacket() override {
            //TODO: Update later
            return 1;
        }

        int parsePacket() override {
            return this->available();
        }

        IPAddress remoteIP() override {
            return this->_remoteIP;
        };

        uint16_t remotePort() override {
            return this->_remotePort;
        };

        /*
        * Extended API
        */
    protected:
        // Read and dump anything remaining in the modem's internal buffer.
        // Using this in the client stop() function.
        // The socket will appear open in response to connected() even after it
        // closes until all data is read from the buffer.
        // Doing it this way allows the external mcu to find and get all of the
        // data that it wants from the socket even if it was closed externally.
        inline
        void dumpModemBuffer(uint32_t maxWaitMs) {
#if defined(TINY_GSM_BUFFER_READ_AND_CHECK_SIZE) || \
    defined(TINY_GSM_BUFFER_READ_NO_CHECK)
            TINY_GSM_YIELD();
            uint32_t startMillis = millis();
            while ((this->sock_available > 0) && ((millis() - startMillis) < maxWaitMs)) {
                this->rx.clear();
                this->at->modemRead(TinyGsmMin((uint16_t)this->rx.free(), this->sock_available), this->mux);
            }
            this->rx.clear();
            this->at->streamClear();

#elif defined(TINY_GSM_NO_MODEM_BUFFER)
            this->rx.clear();
            this->at->streamClear();

#else
            #error "Modem client has been incorrectly created"
#endif
        }

        modemType* at;
        uint8_t    mux;
        uint16_t   sock_available;
        uint32_t   prev_check;
        bool       sock_connected;
        bool       got_data;
        bool       isUDP;
        RxFifo     rx;
        IPAddress  _remoteIP;
        uint16_t   _remotePort;
    };

    /*
    * Basic functions
    */
protected:
    void maintainImpl() {
#if defined(TINY_GSM_BUFFER_READ_AND_CHECK_SIZE)
        // Keep listening for modem URC's and proactively iterate through
        // sockets asking if any data is available
        for (int mux = 0; mux < muxCount; mux++) {
            GsmClient* sock = this->thisModem().sockets[mux];
            if (sock && sock->got_data) {
                sock->got_data       = false;
                sock->sock_available = this->thisModem().modemGetAvailable(mux);
            }
        }
        while (this->thisModem().stream.available()) {
            this->thisModem().waitResponse(15, NULL, NULL);
        }

#elif defined(TINY_GSM_NO_MODEM_BUFFER) ||      \
    defined(TINY_GSM_BUFFER_READ_NO_CHECK)
        // Just listen for any URC's
        this->thisModem().waitResponse(100, NULL, NULL);

#else
        #error "Modem client has been incorrectly created"
#endif
    }

    // Yields up to a time-out period and then reads a character from the stream
    // into the mux FIFO
    // TODO:  Do we need to wait two _timeout periods for no
    // character return?  Will wait once in the first "while
    // !stream.available()" and then will wait again in the stream.read()
    // function.
    inline
    void moveCharFromStreamToFifo(uint8_t mux) {
        if (!this->thisModem().sockets[mux]) {
            return;
        }
        uint32_t startMillis = millis();
        while (!this->thisModem().stream.available() &&
                ((millis() - startMillis) < this->thisModem().sockets[mux]->Client::_timeout)) {
            TINY_GSM_YIELD();
        }
        char c = this->thisModem().stream.read();
        this->thisModem().sockets[mux]->rx.put(c);
    }
};

#endif /* SRC_TINYGSM_TCP_UDP_HPP_ */
