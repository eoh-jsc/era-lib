#ifndef INC_ERA_HELPERS_DEFINE_HPP_
#define INC_ERA_HELPERS_DEFINE_HPP_

#define LOC_BUFFER_PARSE                \
    char locBuf[32] {0};                \
    char* buf = locBuf;                 \
    if (len >= sizeof(locBuf)) {        \
        buf = (char*)malloc(len + 1);   \
        if (buf == nullptr) {           \
            ERA_ASSERT(buf != nullptr); \
            return;                     \
        }                               \
        memset(buf, 0, len + 1);        \
    }

#define FREE_BUFFER_PARSE               \
    if (buf != locBuf) {                \
        free(buf);                      \
    }                                   \
    buf = nullptr;

#endif /* INC_ERA_HELPERS_DEFINE_HPP_ */
