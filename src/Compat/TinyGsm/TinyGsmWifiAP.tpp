/**
 * @file       TinyGsmWifiAP.tpp
 */

#ifndef SRC_TINYGSM_WIFI_AP_HPP_
#define SRC_TINYGSM_WIFI_AP_HPP_

#include <TinyGsmCommon.h>
#include <Utility/ERaQueue.hpp>

#define TINY_GSM_MODEM_HAS_WIFI_AP

enum TinyWiFiEncTypeT {
    WIFI_ENC_TYPE_OPEN = 0x00,
    WIFI_ENC_TYPE_WEP = 0x01,
    WIFI_ENC_TYPE_WPA_PSK = 0x02,
    WIFI_ENC_TYPE_WPA2_PSK = 0x03,
    WIFI_ENC_TYPE_WPA_WPA2_PSK = 0x04,
    WIFI_ENC_TYPE_WPA2_ENTERPRISE = 0x05,
    WIFI_ENC_TYPE_WPA3_PSK = 0x06,
    WIFI_ENC_TYPE_WPA2_WPA3_PSK = 0x07,
    WIFI_ENC_TYPE_WAPI_PSK = 0x08
};

typedef struct __TinyWiFi_t {
    uint16_t encType;
    int8_t rssi;
    uint8_t channel;
    char ssid[33];
} TinyWiFi_t;

template <class modemType>
class TinyGsmWifiAP {
public:
    TinyGsmWifiAP()
        : _localIPAP(0, 0, 0, 0)
    {}

    /*
    * WiFi AP functions
    */
    bool configAP(IPAddress localIP) {
        return this->configAP(localIP, localIP);
    }

    bool configAP(IPAddress localIP, IPAddress gatewayIP,
                IPAddress netmaskIP = IPAddress(255, 255, 255, 0)) {
        this->_localIPAP = localIP;
        return this->thisModem().configAPImpl(TinyGsmStringFromIp(localIP).c_str(),
                                            TinyGsmStringFromIp(gatewayIP).c_str(),
                                            TinyGsmStringFromIp(netmaskIP).c_str());
    }

    bool beginAP(const char* ssid) {
        return this->thisModem().beginAPImpl(ssid);
    }

    bool beginAP(const char* ssid, const char* pwd) {
        return this->thisModem().beginAPImpl(ssid, pwd);
    }

    bool endAP() {
        return this->thisModem().endAPImpl();
    }

    int scanNetworks() {
        this->scanDelete();
        this->thisModem().scanNetworksImpl();
        return this->listWiFi.size();
    }

    void scanDelete() {
        const ERaList<TinyWiFi_t*>::iterator* e = this->listWiFi.end();
        for (ERaList<TinyWiFi_t*>::iterator* it = this->listWiFi.begin(); it != e; it = it->getNext()) {
            if (it->get() == NULL) {
                continue;
            }
            delete it->get();
            it->get() = NULL;
        }
        this->listWiFi.clear();
    }

    uint16_t encryptionType(uint8_t id) {
        ERaList<TinyWiFi_t*>::iterator* info = this->listWiFi[id];
        if (info == NULL) {
            return 0;
        }
        return info->get()->encType;
    }

    int8_t RSSI(uint8_t id) {
        ERaList<TinyWiFi_t*>::iterator* info = this->listWiFi[id];
        if (info == NULL) {
            return 0;
        }
        return info->get()->rssi;
    }

    uint8_t channel(uint8_t id) {
        ERaList<TinyWiFi_t*>::iterator* info = this->listWiFi[id];
        if (info == NULL) {
            return 0;
        }
        return info->get()->channel;
    }

    String SSID() {
        return this->thisModem().SSIDImpl();
    }

    String SSID(uint8_t id) {
        ERaList<TinyWiFi_t*>::iterator* info = this->listWiFi[id];
        if (info == NULL) {
            return String("");
        }
        return String(info->get()->ssid);
    }

    String BSSID() {
        return this->thisModem().BSSIDImpl();
    }

    IPAddress localIPAP() const {
        return this->_localIPAP;
    }

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
    * WiFi AP functions
    */
    bool configAPImpl(const char* localIP, const char* gatewayIP,
                    const char* netmaskIP) TINY_GSM_ATTR_NOT_IMPLEMENTED;
    bool beginAPImpl(const char* ssid) TINY_GSM_ATTR_NOT_IMPLEMENTED;
    bool beginAPImpl(const char* ssid, const char* pwd) TINY_GSM_ATTR_NOT_IMPLEMENTED;
    bool endAPImpl() TINY_GSM_ATTR_NOT_IMPLEMENTED;

    IPAddress _localIPAP;
    ERaList<TinyWiFi_t*> listWiFi;
};

#endif  /* SRC_TINYGSM_WIFI_AP_HPP_ */
