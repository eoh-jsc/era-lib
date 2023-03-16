#include <ERa/ERaHooks.hpp>
#include <Utility/ERaLoc.hpp>

void* era_malloc(size_t size) {
    return ERaHooks::malloc(size);
}

void era_free(void* pointer) {
    ERaHooks::free(pointer);
}

void* era_realloc(void* pointer, size_t size) {
    return ERaHooks::realloc(pointer, size);
}

void* era_calloc(size_t len, size_t size) {
    return ERaHooks::calloc(len, size);
}
