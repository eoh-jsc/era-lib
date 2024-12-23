#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <functional>
#include <type_traits>
#include <Utility/ERaUtility.hpp>

#include <Automation/ERaDefine.hpp>

namespace eras {

    bool StringReplace(std::string& str, const std::string& from, const std::string& to);

    class Mutex
    {
    public:
        Mutex();
        Mutex(const Mutex&) = delete;

        void lock();
        bool tryLock();
        void unlock();

        Mutex& operator = (const Mutex&) = delete;

    private:
        ERaMutex_t mMutex;
    };

    class LockGuard
    {
    public:
        LockGuard(Mutex& mutex)
            : mMutex(mutex)
        {
            this->mMutex.lock();
        }

        ~LockGuard() {
            this->mMutex.unlock();
        }

    private:
        Mutex& mMutex;
    };

    template <typename T>
    class Deduplicator
    {
    public:
        bool next(T value) {
            if (this->mHasValue) {
                if (this->mLastValue == value) {
                    return false;
                }
            }
            this->mHasValue = true;
            this->mLastValue = value;
            return true;
        }

        bool hasValue() const {
            return this->mHasValue;
        }

        T const& value() const {
            return this->mLastValue;
        }

        T& value() {
            return this->mLastValue;
        }

    protected:
        bool mHasValue {false};
        T mLastValue {};
    };

    double ParseNumber(const char* str) {
        char* end = nullptr;
        double value = ::strtod(str, &end);
        if ((end == str) || (*end != '\0') || (value == HUGE_VAL)) {
            return NAN;
        }
        return value;
    }

} /* namespace eras */

#include <Automation/ERaHelpers.tpp>
