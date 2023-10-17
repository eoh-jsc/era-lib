#ifndef INC_CLIENT_HPP__
#define INC_CLIENT_HPP__

#if defined(LINUX)
    #include <Utility/Compat/Client.hpp>
#elif defined(ARDUINO) && \
    defined(ARDUINO_ARCH_ARM)
    #include <api/Client.h>
#else
    #include <Client.h>
#endif

#endif /* INC_CLIENT_HPP__ */