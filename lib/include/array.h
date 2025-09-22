#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

void *allocate_array(size_t size, size_t count);
void destroy_array(void *data);

#endif // ARRAY_H
