#ifndef INC_ERA_CONNECTED_HPP_
#define INC_ERA_CONNECTED_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ERa/ERaDetect.hpp>
#include <Client.hpp>

class ERaConnected
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<bool(void)> NetworkCallback_t;
#else
    typedef bool (*NetworkCallback_t)(void);
#endif

public:
    enum PriorityTypeT
        : uint8_t {
        PRIORITY_BEFORE_WIFI = 0,
        PRIORITY_AFTER_WIFI = 1,
    };

public:
    ERaConnected()
        : mNext(NULL)
        , mClient(NULL)
        , mSSID("")
        , mNetworkCallback(NULL)
        , mPriorityType(PRIORITY_BEFORE_WIFI)
    {
        this->mNetworkCallback = ERaConnected::networkCallbackDummy;
    }
    virtual ~ERaConnected()
    {}

    void setNetworkCallback(NetworkCallback_t callback) {
        this->mNetworkCallback = callback;
    }

    bool isBeforeWiFi() const {
        return (this->mPriorityType == PRIORITY_BEFORE_WIFI);
    }

    bool isAfterWiFi() const {
        return (this->mPriorityType == PRIORITY_AFTER_WIFI);
    }

    PriorityTypeT getPriorityType() const {
        return this->mPriorityType;
    }

    void setPriorityType(PriorityTypeT priorityType) {
        this->mPriorityType = priorityType;
    }

    Client* getClient() const {
        return this->mClient;
    }

    void setClient(Client* client) {
        this->mClient = client;
    }

    const char* getSSID() const {
        return this->mSSID;
    }

    void setSSID(const char* ssid) {
        this->mSSID = ssid;
    }

    ERaConnected* getNext() const {
        return this->mNext;
    }

    void setNext(ERaConnected& rConn) {
        this->mNext = &rConn;
    }

    void setNext(ERaConnected* pConn) {
        this->mNext = pConn;
    }

    bool connected(PriorityTypeT priorityType) const {
        if (this->mPriorityType != priorityType) {
            return false;
        }
        return this->mNetworkCallback();
    }

protected:
private:
    static inline
    bool networkCallbackDummy() {
        return false;
    }

    ERaConnected* mNext;

    Client* mClient;
    const char* mSSID;
    NetworkCallback_t mNetworkCallback;

    PriorityTypeT mPriorityType;
};

#endif /* INC_ERA_CONNECTED_HPP_ */
