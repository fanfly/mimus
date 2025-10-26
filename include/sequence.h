#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stddef.h>

void *sequence_create(size_t element_size);
size_t sequence_count(void *data);
void *sequence_append(void *data, void *element);
void sequence_destroy(void *data);

#endif // SEQUENCE_H