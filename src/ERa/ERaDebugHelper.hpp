#ifndef INC_ERA_DEBUG_HELPER_HPP_
#define INC_ERA_DEBUG_HELPER_HPP_

#if defined(LINUX)
    #define ERA_SERIAL_PRINT(buf)    std::cout << buf
#else
    #define ERA_SERIAL_PRINT(buf)    ERA_SERIAL.print(buf);
#endif

class LogHelper {
public:
    LogHelper()
    {}
    ~LogHelper()
    {}

    template <typename T, typename... Args>
    void print(const T last) {
        ERA_SERIAL_PRINT(last);
        ERA_SERIAL_PRINT("\r\n");
    }

    template <typename T, typename... Args>
    void print(const T head, Args... tail) {
        ERA_SERIAL_PRINT(head);
        this->print(tail...);
    }

    static LogHelper& instance() {
        static LogHelper _instance;
        return _instance;
    }

private:
};

#endif /* INC_ERA_DEBUG_HELPER_HPP_ */
