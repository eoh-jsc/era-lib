#ifndef INC_ERA_BLE_VERIFY_HPP_
#define INC_ERA_BLE_VERIFY_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Utility/ERaUtility.hpp>

#if !defined(ERA_MAX_BLE_CONNECTION)
    #define ERA_MAX_BLE_CONNECTION  5
#endif

#define ERA_VERIFY_EXPIRE_TIME      60000

class ERaBLEVerify
{
    typedef struct __BLEVerify_t {
        bool enabled;
        bool isVerified;
        uint16_t connID;
        ERaInt_t verifyCode;
        unsigned long prevMillis;
    } BLEVerify_t;
    const static int MAX_BLE_CONNECTION = ERA_MAX_BLE_CONNECTION;

public:
    class iterator
    {
    public:
        iterator()
            : pVerify(NULL)
            , index(0)
        {}
        iterator(BLEVerify_t* verify, int index)
            : pVerify(verify)
            , index(index)
        {}
        ~iterator()
        {}

        bool isValid() const {
            return ((this->pVerify != NULL) && (this->index < MAX_BLE_CONNECTION));
        }

        ERaInt_t getConnID() const {
            if (!this->isValid()) {
                return -1;
            }
            if (!this->pVerify[this->index].enabled) {
                return -1;
            }
            return this->pVerify[this->index].connID;
        }

        bool operator == (const iterator& it) const {
            return ((this->pVerify == it.pVerify) && (this->index == it.index));
        }

        bool operator != (const iterator& it) const {
            return ((this->pVerify != it.pVerify) || (this->index != it.index));
        }

        iterator const& operator * () const {
            return (*this);
        }

        iterator& operator * () {
            return (*this);
        }

        iterator& operator ++ () {
            if (this->isValid()) {
                this->index++;
            }
            return (*this);
        }
    private:
        BLEVerify_t* pVerify;
        unsigned int index;
    };

    ERaBLEVerify()
        : numVerify(0U)
    {
        memset(this->verify, 0, sizeof(this->verify));
    }
    ~ERaBLEVerify()
    {}

    bool isVerified(uint16_t connID) {
        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                continue;
            }
            if (this->verify[i].connID == connID) {
                return this->verify[i].isVerified;
            }
        }
        return false;
    }

    void setVerified(uint16_t connID, bool verified) {
        if (!verified) {
            return this->removeVerify(connID);
        }

        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                continue;
            }
            if (this->verify[i].connID == connID) {
                this->verify[i].isVerified = true;
                break;
            }
        }
    }

    void removeVerify(uint16_t connID) {
        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                continue;
            }
            if (this->verify[i].connID == connID) {
                this->deleteVerify(i);
                break;
            }
        }
    }

    ERaInt_t getVerifyCode(uint16_t connID, bool getOnly = false) {
        ERaInt_t verifyCode = this->findVerifyCode(connID);
        if (verifyCode > 0) {
            return verifyCode;
        }
        if (getOnly) {
            return -1;
        }

        int id = this->findVerifyFree();
        if (id < 0) {
            return -1;
        }

        this->verify[id].enabled = true;
        this->verify[id].isVerified = false;
        this->verify[id].connID = connID;
        this->verify[id].verifyCode = ERaRandomNumber(10000000, 99999999);
        this->verify[id].prevMillis = ERaMillis();
        this->numVerify++;
        return this->verify[id].verifyCode;
    }

    ERaInt_t connTimeout() {
        unsigned long currentMillis = ERaMillis();
        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                continue;
            }
            if (this->verify[i].isVerified) {
                continue;
            }
            if ((currentMillis - this->verify[i].prevMillis) > ERA_VERIFY_EXPIRE_TIME) {
                this->deleteVerify(i);
                return this->verify[i].connID;
            }
        }
        return -1;
    }

    bool hasVerify() const {
        return (this->numVerify > 0);
    }

    iterator begin() {
        return iterator(this->verify, 0);
    }

    iterator end() {
        return iterator(this->verify, MAX_BLE_CONNECTION);
    }

protected:
private:
    void deleteVerify(unsigned int id) {
        this->verify[id].enabled = false;
        this->numVerify--;
    }

    ERaInt_t findVerifyCode(uint16_t connID) {
        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                continue;
            }
            if (this->verify[i].connID == connID) {
                return this->verify[i].verifyCode;
            }
        }
        return -1;
    }

    int findVerifyFree() {
        if (this->numVerify >= MAX_BLE_CONNECTION) {
            return -1;
        }

        for (int i = 0; i < MAX_BLE_CONNECTION; ++i) {
            if (!this->isValidVerify(i)) {
                return i;
            }
        }
        return -1;
    }

    bool isValidVerify(unsigned int id) {
        return this->verify[id].enabled;
    }

    BLEVerify_t verify[MAX_BLE_CONNECTION];
    unsigned int numVerify;
};

#endif /* INC_ERA_BLE_VERIFY_HPP_ */
