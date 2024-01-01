#ifndef INC_ERA_ETHERNET_ESP32_CLIENT_IDF_5_HPP_
#define INC_ERA_ETHERNET_ESP32_CLIENT_IDF_5_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Ethernet"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#include <ETH.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>

#ifndef ETH_PHY_ADDR
    #define ETH_PHY_ADDR 0
#endif

#ifndef ETH_PHY_TYPE
    #define ETH_PHY_TYPE ETH_PHY_LAN8720
#endif

#ifndef ETH_PHY_POWER
    #define ETH_PHY_POWER -1
#endif

#ifndef ETH_PHY_MDC
    #define ETH_PHY_MDC 23
#endif

#ifndef ETH_PHY_MDIO
    #define ETH_PHY_MDIO 18
#endif

#ifndef ETH_CLK_MODE
    #define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
#endif

template <class Transport>
class ERaEthernet
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "Ethernet";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaEthernet(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaEthernet()
    {}

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->getTransp().config(host, port, username, password);
    }

    void config(IPAddress localIP,
                IPAddress gateway,
                IPAddress subnet,
                IPAddress dns1 = (uint32_t)0x00000000,
                IPAddress dns2 = (uint32_t)0x00000000) {
        ETH.config(localIP, gateway, subnet, dns1, dns2);
    }

#if CONFIG_ETH_USE_ESP32_EMAC
    bool connectNetwork(uint8_t phyAddr = ETH_PHY_ADDR,
                        int power = ETH_PHY_POWER,
                        int mdc = ETH_PHY_MDC,
                        int mdio = ETH_PHY_MDIO,
                        eth_phy_type_t type = ETH_PHY_LAN8720,
                        eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        if (!ETH.begin(type, phyAddr, mdc,
                       mdio, power, clkMode)){
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect failed"));
            return false;
        }
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        IPAddress localIP = ETH.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("Connected to network"));
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
        return true;
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int power,
                int mdc,
                int mdio,
                eth_phy_type_t type,
                eth_clock_mode_t clkMode,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(phyAddr, power, mdc, mdio, type, clkMode);
    }

    void begin(const char* auth,
                uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, phyAddr, power,
                    mdc, mdio, type, clkMode,
                    host, port, auth, auth);
    }

    void begin(uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    phyAddr, power,
                    mdc, mdio,
                    type, clkMode,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }
#endif

    /* Ethernet Arduino SPI */
    bool connectNetwork(uint8_t phyAddr,
                        int cs,
                        int irq,
                        int rst,
                        SPIClass &spi,
                        eth_phy_type_t type = ETH_PHY_W5500,
                        uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        if (!ETH.begin(type, phyAddr, cs,
                       irq, rst, spi, spiFreq)){
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect failed"));
            return false;
        }
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        IPAddress localIP = ETH.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("Connected to network"));
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
        return true;
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                SPIClass &spi,
                eth_phy_type_t type,
                uint8_t spiFreq,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(phyAddr, cs, irq, rst, spi, type, spiFreq);
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                SPIClass &spi,
                eth_phy_type_t type = ETH_PHY_W5500,
                uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, phyAddr, cs,
                    irq, rst, spi, type, spiFreq,
                    host, port, auth, auth);
    }

    void begin(uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                SPIClass &spi,
                eth_phy_type_t type = ETH_PHY_W5500,
                uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ) {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    phyAddr, cs,
                    irq, rst, spi,
                    type, spiFreq,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    /* Ethernet IDF SPI */
    bool connectNetwork(uint8_t phyAddr,
                        int cs,
                        int irq,
                        int rst,
                        spi_host_device_t spiHost,
                        int sck = -1,
                        int miso = -1,
                        int mosi = -1,
                        eth_phy_type_t type = ETH_PHY_W5500,
                        uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        if (!ETH.begin(type, phyAddr, cs,
                       irq, rst, spiHost,
                       sck, miso, mosi, spiFreq)){
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect failed"));
            return false;
        }
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        IPAddress localIP = ETH.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("Connected to network"));
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
        return true;
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                spi_host_device_t spiHost,
                int sck,
                int miso,
                int mosi,
                eth_phy_type_t type,
                uint8_t spiFreq,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(phyAddr, cs, irq, rst, spiHost,
                            sck, miso, mosi, type, spiFreq);
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                spi_host_device_t spiHost,
                int sck,
                int miso,
                int mosi,
                eth_phy_type_t type = ETH_PHY_W5500,
                uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, phyAddr, cs,
                    irq, rst, spiHost, sck,
                    miso, mosi, type, spiFreq,
                    host, port, auth, auth);
    }

    void begin(uint8_t phyAddr,
                int cs,
                int irq,
                int rst,
                spi_host_device_t spiHost,
                int sck,
                int miso,
                int mosi,
                eth_phy_type_t type = ETH_PHY_W5500,
                uint8_t spiFreq = ETH_PHY_SPI_FREQ_MHZ) {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    phyAddr, cs,
                    irq, rst, spiHost,
                    sck, miso, mosi,
                    type, spiFreq,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect()) {
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                else {
                    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            default:
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                break;
        }
    }

protected:
private:
    const char* authToken;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, ETH.linkSpeed());
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ETH.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ETH.localIP().toString().c_str());
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason().c_str());
#endif

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, ETH.linkSpeed());
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "Ethernet");

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_ETHERNET_ESP32_CLIENT_IDF_5_HPP_ */
