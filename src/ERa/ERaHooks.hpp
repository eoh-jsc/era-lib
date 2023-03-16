#ifndef INC_ERA_HOOKS_HPP_
#define INC_ERA_HOOKS_HPP_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

class ERaHooks
{
    typedef void *(*allocate_t)(size_t size);
    typedef void *(*reallocate_t)(void* pointer, size_t size);
    typedef void *(*calloc_t)(size_t len, size_t size);
    typedef void (*deallocate_t)(void* pointer);

public:
#if defined(RTL8722DM) || defined(ARDUINO_AMEBA)
    ERaHooks()
        : malloc_fn(::malloc)
        , realloc_fn(::realloc)
        , calloc_fn([](size_t len, size_t size) -> void* {
            return ::calloc(len, size);
        })
        , free_fn(::free)
    {}
#else
    ERaHooks()
        : malloc_fn(::malloc)
        , realloc_fn(::realloc)
        , calloc_fn(::calloc)
        , free_fn(::free)
    {}
#endif

    static inline
    void setHooks(allocate_t _malloc_fn, reallocate_t _realloc_fn,
                calloc_t _calloc_fn, deallocate_t _free_fn) {
        ERaHooks::instance().malloc_fn = _malloc_fn;
        ERaHooks::instance().realloc_fn = _realloc_fn;
        ERaHooks::instance().calloc_fn = _calloc_fn;
        ERaHooks::instance().free_fn = _free_fn;
    }

    static inline
    void* malloc(size_t size) {
        return ERaHooks::instance().malloc_fn(size);
    }

    static inline
    void* realloc(void* pointer, size_t size) {
        return ERaHooks::instance().realloc_fn(pointer, size);
    }

    static inline
    void* calloc(size_t len, size_t size) {
        if (ERaHooks::instance().calloc_fn != nullptr) {
            return ERaHooks::instance().calloc_fn(len, size);
        }

        void* ptr = nullptr;
        size_t totalSize = len * size;

        ptr = ERaHooks::malloc(totalSize);
        if (ptr != nullptr) {
            memset(ptr, 0, totalSize);
        }

        return ptr;
    }

    static inline
    void free(void* pointer) {
        ERaHooks::instance().free_fn(pointer);
    }

private:
    static ERaHooks& instance() {
        static ERaHooks _instance;
        return _instance;
    }

    allocate_t malloc_fn;
    reallocate_t realloc_fn;
    calloc_t calloc_fn;
    deallocate_t free_fn;
};

#endif /* INC_ERA_HOOKS_HPP_ */
