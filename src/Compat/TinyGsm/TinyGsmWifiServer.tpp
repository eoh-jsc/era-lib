/**
 * @file       TinyGsmWifiServer.tpp
 */

#ifndef SRC_TINYGSM_WIFI_SERVER_HPP_
#define SRC_TINYGSM_WIFI_SERVER_HPP_

#include <TinyGsmCommon.h>

#define TINY_GSM_MODEM_HAS_WIFI_SERVER

template <class modemType>
class TinyGsmWifiServer {
public:
    TinyGsmWifiServer()
    {}

    /*
    * WiFi Server functions
    */
    bool beginServer(uint16_t port) {
        return this->thisModem().beginServerImpl(port);
    }

    bool endServer() {
        return this->thisModem().endServerImpl();
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
    * WiFi Server functions
    */
    bool beginServerImpl(uint16_t port) TINY_GSM_ATTR_NOT_IMPLEMENTED;
    bool endServerImpl() TINY_GSM_ATTR_NOT_IMPLEMENTED;
};

#endif /* SRC_TINYGSM_WIFI_SERVER_HPP_ */
