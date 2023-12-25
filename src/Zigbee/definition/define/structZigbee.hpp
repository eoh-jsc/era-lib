#ifndef INC_DEFINE_STRUCT_ZIGBEE_HPP_
#define INC_DEFINE_STRUCT_ZIGBEE_HPP_

#include <string.h>
#include <functional>
#include <Zigbee/definition/ERaDefineZigbee.hpp>

namespace ZigbeeNamespace {

    typedef struct __CompareRsp_t
        : eras::binary_function<Response_t, Response_t, bool> {
        const Response_t& rsp;
        const Response_t& wait;
        __CompareRsp_t(const Response_t& _rsp, const Response_t& _wait)
            : rsp(_rsp)
            , wait(_wait)
        {}
        operator bool () const {
            if (wait.subSystem == SubsystemT::AF_INTER && wait.command == AFCommandsT::AF_INCOMING_MSG) {
                return ((rsp.nwkAddr == wait.nwkAddr) && (rsp.type == wait.type) &&
                        (rsp.subSystem == wait.subSystem) && (rsp.command == wait.command) &&
                        (rsp.transIdZcl == wait.transIdZcl));
            }
            return ((rsp.nwkAddr == wait.nwkAddr) && (rsp.type == wait.type) &&
                    (rsp.subSystem == wait.subSystem) && (rsp.command == wait.command) &&
                    (rsp.transId == wait.transId));
        }
    } CompareRsp_t;

    typedef struct __CheckAFdata_t
        : eras::binary_function<Response_t, Response_t, bool> {
        const Response_t& rsp;
        const Response_t& wait;
        __CheckAFdata_t(const Response_t& _rsp, const Response_t& _wait)
            : rsp(_rsp)
            , wait(_wait)
        {}
        operator bool () const {
            return ((rsp.transId == wait.transId) && (rsp.type == wait.type) &&
                    (rsp.subSystem == wait.subSystem) && (rsp.type == TypeT::AREQ) &&
                    (rsp.subSystem == SubsystemT::AF_INTER) && (rsp.command == AFCommandsT::AF_DATA_CONFIRM));
        }
    } CheckAFdata_t;

    typedef struct __CheckRsp_t
        : eras::binary_function<Response_t, Response_t, bool> {
        const Response_t& rsp;
        const Response_t& wait;
        __CheckRsp_t(const Response_t& _rsp, const Response_t& _wait)
            : rsp(_rsp)
            , wait(_wait)
        {}
        operator bool () const {
            if (wait.subSystem == SubsystemT::AF_INTER) {
                if (wait.command == AFCommandsT::AF_DATA_CONFIRM) {
                    return ((rsp.transId != wait.transId) && (rsp.subSystem == SubsystemT::AF_INTER) &&
                            (rsp.command == AFCommandsT::AF_DATA_CONFIRM));
                }
                else if (wait.command == AFCommandsT::AF_INCOMING_MSG) {
                    if (rsp.subSystem == SubsystemT::AF_INTER && rsp.command == AFCommandsT::AF_DATA_CONFIRM) {
                        return rsp.transId != wait.transId;
                    }
                    return ((rsp.transIdZcl != wait.transIdZcl) && (rsp.subSystem == SubsystemT::AF_INTER) &&
                            (rsp.command == AFCommandsT::AF_INCOMING_MSG));
                }
            }
            return false;
        }
    } CheckRsp_t;
        
    typedef struct __CheckInfoCoordinator_t
        : eras::unary_function<InfoCoordinator_t, bool> {
        const InfoCoordinator_t* const coord;
        __CheckInfoCoordinator_t(const InfoCoordinator_t* const _coord)
            : coord(_coord)
        {}
        operator bool () const {
            if (coord->hasConfigured != FLAG_ZIGBEE_HAS_CONFIGURED) {
                return false;
            }
            else {
                if (coord->channel != NO_LOAD_CHANNEL && coord->address.panId != 1) {
                    return ((coord->channel != CHANNEL_NONE) && (coord->address.panId != 0) &&
                            (coord->address.panId != 0xFFFF) && (coord->nIB.nwkPanId != 0) &&
                            (coord->nIB.nwkPanId != 0xFFFF) && (coord->nIB.nwkLogicalChannel != 0));
                }
                else {
                    return true;
                }
            }
        }
    } CheckInfoCoordinator_t;

    typedef struct __find_deviceWithNwkAddr_t
        : eras::unary_function<IdentDeviceAddr_t, bool> {
        uint16_t nwkAddr;
        __find_deviceWithNwkAddr_t(uint16_t _nwkAddr) : nwkAddr(_nwkAddr) { }
        bool operator()(IdentDeviceAddr_t const& e) const {
            return e.address.addr.nwkAddr == nwkAddr;
        }
    } find_deviceWithNwkAddr_t;

    typedef struct __find_deviceWithIEEEAddr_t
        : eras::unary_function<IdentDeviceAddr_t, bool> {
        const uint8_t* ieeeAddr;
        __find_deviceWithIEEEAddr_t(const uint8_t* _ieeeAddr) : ieeeAddr(_ieeeAddr) { }
        bool operator()(IdentDeviceAddr_t const& e) const {
            return !memcmp(e.address.addr.ieeeAddr, ieeeAddr, LENGTH_EXTADDR_IEEE);
        }
    } find_deviceWithIEEEAddr_t;

    typedef struct __find_endpoint_t
        : eras::unary_function<InfoEndpoint_t, bool> {
        EndpointListT endpoint;
        __find_endpoint_t(EndpointListT _endpoint) : endpoint(_endpoint) { }
        bool operator()(InfoEndpoint_t const& e) const {
            return e.endpoint == endpoint;
        }
    } find_endpoint_t;

    typedef struct __find_devicePayloadWithTopic_t
        : eras::unary_function<IdentDeviceAddr_t, bool> {
        const char* topic;
        __find_devicePayloadWithTopic_t(const char* _topic)
            : topic(_topic)
        {}
        bool operator () (IdentDeviceAddr_t const& e) const {
            if (this->topic == nullptr) {
                return false;
            }
            if (e.data.topic == nullptr) {
                return false;
            }
            return !strcmp(e.data.topic, this->topic);
        }
    } find_devicePayloadWithTopic_t;

};

#endif /* INC_DEFINE_STRUCT_ZIGBEE_HPP_ */
