#ifndef INC_ERA_HELPERS_HPP_
#define INC_ERA_HELPERS_HPP_

#include <ERa/ERaDetect.hpp>

template <class S, typename... Args>
class StaticHelper {
public:
    static inline
    S& instance(Args... tail) {
        static S _instance(tail...);
        return _instance;
    }
};

template <class S, typename... Args>
class StaticRefHelper {
public:
    static inline
    S& instance(Args&... tail) {
        static S _instance(tail...);
        return _instance;
    }
};

template <class S>
class PrintHelper {
public:
    PrintHelper(S& _stream)
        : stream(_stream)
    {}
    ~PrintHelper()
    {}

    template <typename T, typename... Args>
    void print(const T last) {
        this->stream.print(last);
    }

    template <typename T, typename... Args>
    void print(const T head, Args... tail) {
        this->stream.print(head);
        this->print(tail...);
    }

private:
    S& stream;
};

#if defined(ERA_HAS_TYPE_TRAITS_H)

    /* std::enable_if_t from C++14 */
    #if __cplusplus >= 201402L
        using std::enable_if_t;
    #else
        template <bool B, class T = void>
        using enable_if_t = typename std::enable_if<B, T>::type;
    #endif

#endif

#endif /* INC_ERA_HELPERS_HPP_ */
