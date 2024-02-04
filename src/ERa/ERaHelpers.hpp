#ifndef INC_ERA_HELPERS_HPP_
#define INC_ERA_HELPERS_HPP_

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

#endif /* INC_ERA_HELPERS_HPP_ */
