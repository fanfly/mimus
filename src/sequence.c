#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sequence.h"

struct sequence {
    size_t size;
    size_t count;
    size_t capacity;
    void *data;
};

struct sequence *sequence_create(size_t size) {
    struct sequence *sequence = malloc(sizeof(*sequence));
    sequence->size = size;
    sequence->count = 0;
    sequence->capacity = 0;
    sequence->data = NULL;
    return sequence;
}

size_t sequence_count(struct sequence *sequence) {
    return sequence->count;
}

void *sequence_get(struct sequence *sequence, size_t index) {
    return (unsigned char *)sequence->data + sequence->size * index;
}

void sequence_reallocate(struct sequence *sequence, size_t capacity) {
    sequence->data = realloc(sequence->data, sequence->size * capacity);
    sequence->capacity = capacity;
}

void sequence_append(struct sequence *sequence, void *element) {
    if (sequence->count == sequence->capacity) {
        size_t new_capacity = sequence->count == 0 ? 1 : sequence->count * 2;
        sequence_reallocate(sequence, new_capacity);
    }
    void *target = sequence_get(sequence, sequence->count);
    memcpy(target, element, sequence->size);
    ++sequence->count;
}

void sequence_destroy(struct sequence *sequence) {
    free(sequence->data);
    free(sequence);
}
