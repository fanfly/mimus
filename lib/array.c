#include <stdlib.h>
#include <stddef.h>

#include "array.h"

struct array_header {
    size_t size;
    size_t count;
    size_t capacity;
};

void *allocate_array(size_t size, size_t count) {
    size_t total_size = sizeof(struct array_header) + size * count;
    struct array_header *array = malloc(total_size);
    array->size = size;
    array->count = count;
    array->capacity = count;
    void *data = (unsigned char *)array + sizeof(struct array_header);
    return data;
}

void destroy_array(void *data) {
    void *array = (unsigned char *)data - sizeof(struct array_header);
    free(array);
}
