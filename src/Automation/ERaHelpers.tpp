#include <Automation/ERaHelpers.hpp>

namespace eras {

    bool StringReplace(std::string& str, const std::string& from, const std::string& to) {
        size_t startPos = str.find(from);
        if (startPos == std::string::npos) {
            return false;
        }
        str.replace(startPos, from.length(), to);
        return true;
    }

    Mutex::Mutex() {
    }

    void Mutex::lock() {
        ERaGuardLock(this->mMutex);
    }

    bool Mutex::tryLock() {
        return ERaGuardTryLock(this->mMutex);
    }

    void Mutex::unlock() {
        ERaGuardUnlock(this->mMutex);
    }

} /* namespace eras */
