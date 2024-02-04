#ifndef INC_ERA_API_STATIC_HPP_
#define INC_ERA_API_STATIC_HPP_

#include <ERa/ERaApi.hpp>

#if !defined(ERA_HAS_FUNCTIONAL_H)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::_appLoop() {
        ERa.ERaApi::appLoop();
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::_sendPinEvent(void* args) {
        ERa.ERaApi::sendPinEvent(args);
    }

    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::_sendPinConfigEvent(void* args) {
        ERa.ERaApi::sendPinConfigEvent(args);
    }
#endif

#endif /* INC_ERA_API_STATIC_HPP_ */
