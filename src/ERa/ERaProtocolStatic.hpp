#ifndef INC_ERA_PROTOCOL_STATIC_HPP_
#define INC_ERA_PROTOCOL_STATIC_HPP_

#include <ERa/ERaProtocol.hpp>

#if !defined(ERA_HAS_FUNCTIONAL_H)
    template <class Transp, class Flash>
    inline
    void ERaProto<Transp, Flash>::_onConnected() {
        ERa.ERaProto::onConnected();
    }

    template <class Transp, class Flash>
    inline
    void ERaProto<Transp, Flash>::_onDisconnected() {
        ERa.ERaProto::onDisconnected();
    }

    template <class Transp, class Flash>
    inline
    void ERaProto<Transp, Flash>::_processRequest(const char* topic, const char* payload) {
        ERa.ERaProto::processRequest(topic, payload);
    }
#endif

#endif /* INC_ERA_PROTOCOL_STATIC_HPP_ */
