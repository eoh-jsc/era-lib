#ifndef INC_ERA_OTP_HPP_
#define INC_ERA_OTP_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Utility/ERaUtility.hpp>

#if !defined(ERA_MAX_OTP)
    #define ERA_MAX_OTP         5
#endif

#define ERA_OTP_EXPIRE_TIME     20000

class ERaOTP
{
    typedef struct __OTP_t {
        bool enabled;
        int otp;
        unsigned long prevMillis;
    } OTP_t;
    const static int MAX_OTP = ERA_MAX_OTP;

public:
    ERaOTP()
        : numOTP(0)
    {}
    ~ERaOTP()
    {}

    bool run(int _otp) {
        unsigned long currentMillis = ERaMillis();
        for (int i = 0; i < MAX_OTP; ++i) {
            if (!this->isValidOTP(i)) {
                continue;
            }
            if (currentMillis - this->otp[i].prevMillis < ERA_OTP_EXPIRE_TIME) {
                if (this->otp[i].otp == _otp) {
                    this->deleteOTP(i);
                    return true;
                }
                continue;
            }
            this->deleteOTP(i);
        }
        return false;
    }

    int createOTP() {
        this->run(0);

        if (this->numOTP >= MAX_OTP) {
            return -1;
        }

        int id = this->findOTPFree();
        if (id < 0) {
            return -1;
        }

        this->otp[id].enabled = true;
        this->otp[id].prevMillis = ERaMillis();
        this->otp[id].otp = ERaRandomNumber(100000000, 999999999);
        this->numOTP++;
        return this->otp[id].otp;
    }

protected:
private:
    void deleteOTP(unsigned int id) {
        this->otp[id].enabled = false;
        this->numOTP--;
    }

    int findOTPFree() {
        if (this->numOTP >= MAX_OTP) {
            return -1;
        }

        for (int i = 0; i < MAX_OTP; i++) {
            if (!this->isValidOTP(i)) {
                return i;
            }
        }
        return -1;
    }

    bool isValidOTP(unsigned int id) const {
        return this->otp[id].enabled;
    }

    OTP_t otp[MAX_OTP];
    unsigned int numOTP;
};

#endif /* INC_ERA_OTP_HPP_ */
