#ifndef INC_ERA_STREAM_LINUX_HPP_
#define INC_ERA_STREAM_LINUX_HPP_

#include "Compat/Stream.hpp"

inline
ERaString Stream::readString() {
    ERaString ret;
    int size = this->available();
    if (size <= 0) {
        return ret;
    }

    size = ERaMin(size, 1024);
    ret.reserve(size);

    int c = this->timedRead();
    while (c >= 0) {
        ret += (char)c;
        c = this->timedRead();
    }
    return ret;
}

inline
ERaString Stream::readStringUntil(char terminator) {
    ERaString ret;
    int size = this->available();
    if (size <= 0) {
        return ret;
    }

    size = ERaMin(size, 1024);
    ret.reserve(size);

    int c = this->timedRead();
    while ((c >= 0) && (c != terminator)) {
        ret += (char)c;
        c = this->timedRead();
    }
    return ret;
}

#endif /* INC_ERA_STREAM_LINUX_HPP_ */
