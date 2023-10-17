#ifndef INC_STREAM_HPP__
#define INC_STREAM_HPP__

#if defined(LINUX)
    #include <Utility/Compat/Stream.hpp>
#elif defined(ARDUINO) && \
    defined(ARDUINO_ARCH_ARM)
    #include <api/Stream.h>
#else
    #include <Stream.h>
#endif

#endif /* INC_STREAM_HPP__ */