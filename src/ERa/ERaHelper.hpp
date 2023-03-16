#ifndef INC_ERA_HELPER_HPP_
#define INC_ERA_HELPER_HPP_

template <class S, typename... Args>
class StaticHelper {
public:
    static S& instance(Args... tail) {
        static S _instance(tail...);
        return _instance;
    }
};

template <class S, typename... Args>
class StaticRefHelper {
public:
    static S& instance(Args&... tail) {
        static S& _instance(tail...);
        return _instance;
    }
};

#endif /* INC_ERA_HELPER_HPP_ */
