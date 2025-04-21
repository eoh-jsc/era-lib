#ifndef INC_ERA_CONNECTED_HANDLER_HPP_
#define INC_ERA_CONNECTED_HANDLER_HPP_

#include <ERa/ERaState.hpp>
#include <PnP/ERaConnected.hpp>

template <class Transport>
class ERaConnectedHandler
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<bool(void)> NetworkCallback_t;
#else
    typedef bool (*NetworkCallback_t)(void);
#endif

    using PriorityType = ERaConnected::PriorityTypeT;

public:
    ERaConnectedHandler()
        : mTransport(NULL)
        , mFirstConn(NULL)
        , mLastConn(NULL)
        , mDefaultClient(NULL)
        , mDefaultSSID("")
    {}

    ERaConnectedHandler(Transport& transport)
        : mTransport(&transport)
        , mFirstConn(NULL)
        , mLastConn(NULL)
        , mDefaultClient(NULL)
        , mDefaultSSID("")
    {
        this->getDefault();
    }

    virtual ~ERaConnectedHandler()
    {
        this->clear();
    }

    void setTransport(Transport* transport) {
        this->mTransport = transport;
        this->getDefault();
    }

    void setTransport(Transport& transport) {
        this->mTransport = &transport;
        this->getDefault();
    }

    void setDefaultClient(Client* client) {
        this->mDefaultClient = client;
    }

    void setDefaultClient(Client& client) {
        this->mDefaultClient = &client;
    }

    void setDefaultSSID(const char* ssid) {
        this->mDefaultSSID = ssid;
    }

    bool hasBeforeWiFi() const {
        ERaConnected* conn = this->mFirstConn;
        while (conn != NULL) {
            if (conn->isBeforeWiFi()) {
                return true;
            }
            conn = conn->getNext();
        }
        return false;
    }

    void setNetworkCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                             const char* ssid = nullptr) {
        this->clear(PriorityType::PRIORITY_BEFORE_WIFI);
        this->addBeforeWiFiCallbacks(callback, client, ssid);
    }

    void setNetworkCallbacks(NetworkCallback_t callback, Client& client,
                             const char* ssid = nullptr) {
        this->setNetworkCallbacks(callback, &client, ssid);
    }

    void setGsmCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                         const char* ssid = nullptr) {
        this->clear(PriorityType::PRIORITY_AFTER_WIFI);
        this->addAfterWiFiCallbacks(callback, client, ssid);
    }

    void setGsmCallbacks(NetworkCallback_t callback, Client& client,
                         const char* ssid = nullptr) {
        this->setGsmCallbacks(callback, &client, ssid);
    }

    void addBeforeWiFiCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                                const char* ssid = nullptr) {
        if (client == nullptr) {
            client = this->mDefaultClient;
        }
        if (ssid == nullptr) {
            ssid = this->mDefaultSSID;
        }
        ERaConnected* conn = new ERaConnected();
        conn->setNetworkCallback(callback);
        conn->setClient(client);
        conn->setSSID(ssid);
        conn->setPriorityType(PriorityType::PRIORITY_BEFORE_WIFI);
        this->addConn(conn);
    }

    void addBeforeWiFiCallbacks(NetworkCallback_t callback, Client& client,
                                const char* ssid = nullptr) {
        this->addBeforeWiFiCallbacks(callback, &client, ssid);
    }

    void addAfterWiFiCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                               const char* ssid = nullptr) {
        if (client == nullptr) {
            client = this->mDefaultClient;
        }
        if (ssid == nullptr) {
            ssid = this->mDefaultSSID;
        }
        ERaConnected* conn = new ERaConnected();
        conn->setNetworkCallback(callback);
        conn->setClient(client);
        conn->setSSID(ssid);
        conn->setPriorityType(PriorityType::PRIORITY_AFTER_WIFI);
        this->addConn(conn);
    }

    void addAfterWiFiCallbacks(NetworkCallback_t callback, Client& client,
                               const char* ssid = nullptr) {
        this->addAfterWiFiCallbacks(callback, &client, ssid);
    }

    void clear() {
        this->clear(PriorityType::PRIORITY_BEFORE_WIFI);
        this->clear(PriorityType::PRIORITY_AFTER_WIFI);
    }

    void clear(PriorityType priorityType) {
        ERaConnected* begin = NULL;
        ERaConnected* end = NULL;

        ERaConnected* conn = this->mFirstConn;
        while (conn != NULL) {
            ERaConnected* next = conn->getNext();
            if (conn->getPriorityType() == priorityType) {
                delete conn;
            }
            else if (begin == NULL) {
                begin = conn;
                end = conn;
            }
            else {
                end->setNext(conn);
                end = conn;
            }
            conn = next;
        }

        if (end != NULL) {
            end->setNext(NULL);
        }
        this->mFirstConn = begin;
        this->mLastConn = end;
    }

    bool beforeWiFiConnected(bool skipState = false) const {
        return this->connected(PriorityType::PRIORITY_BEFORE_WIFI, skipState);
    }

    bool afterWiFiConnected(bool skipState = false) const {
        return this->connected(PriorityType::PRIORITY_AFTER_WIFI, skipState);
    }

    bool connected(PriorityType priorityType, bool skipState = false) const {
        ERaConnected* conn = this->mFirstConn;
        while (conn != NULL) {
            if (conn->connected(priorityType)) {
                this->switchClient(conn, skipState);
                return true;
            }
            conn = conn->getNext();
        }
        return false;
    }

    void switchClient(ERaConnected* pConn = nullptr, bool skipState = false) const {
        Client* targetClient = this->mDefaultClient;
        const char* targetSSID = this->mDefaultSSID;
        if (pConn != nullptr) {
            targetClient = pConn->getClient();
            targetSSID = pConn->getSSID();
        }
        Client* currentClient = this->mTransport->getClient();
        if (currentClient == targetClient) {
            return;
        }
        if (currentClient->connected()) {
            this->mTransport->disconnect();
        }
        this->mTransport->setClient(targetClient);
        this->mTransport->setSSID(targetSSID);
        if (skipState) {
            return;
        }
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }

protected:
    void addConn(ERaConnected& rConn) {
        this->addConn(&rConn);
    }

    void addConn(ERaConnected* pConn) {
        if (this->mLastConn == NULL) {
            this->mFirstConn = pConn;
        }
        else {
            this->mLastConn->setNext(pConn);
        }
        this->mLastConn = pConn;
    }

    void updateClient(Client* targetClient) {
        ERaConnected* conn = this->mFirstConn;
        while (conn != NULL) {
            if (conn->getClient() == this->mDefaultClient) {
                conn->setClient(targetClient);
            }
            conn = conn->getNext();
        }
        this->mDefaultClient = targetClient;
        this->mTransport->setClient(targetClient);
    }

private:
    void getDefault() {
        this->mDefaultClient = this->mTransport->getClient();
        this->mDefaultSSID = this->mTransport->getSSID();
    }

    Transport* mTransport;

    ERaConnected* mFirstConn;
    ERaConnected* mLastConn;

    Client* mDefaultClient;
    const char* mDefaultSSID;
};

#endif /* INC_ERA_CONNECTED_HANDLER_HPP_ */
