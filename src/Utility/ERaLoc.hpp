#ifndef INC_ERA_ALLOCATE_HPP_
#define INC_ERA_ALLOCATE_HPP_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* era_malloc(size_t size);
void era_free(void* pointer);
void* era_realloc(void* pointer, size_t size);
void* era_calloc(size_t len, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* INC_ERA_ALLOCATE_HPP_ */
