#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sequence.h"

struct sequence_header {
    size_t element_size;
    size_t count;
    size_t capacity;
};

const size_t SEQ_HEADER_SIZE = sizeof(struct sequence_header);

struct sequence_header *sequence_header(void *data) {
    return (struct sequence_header *)((unsigned char *)data - SEQ_HEADER_SIZE);
}

void *sequence_create(size_t element_size) {
    struct sequence_header *header = malloc(SEQ_HEADER_SIZE);
    header->element_size = element_size;
    header->count = 0;
    header->capacity = 0;
    void *data = (unsigned char *)header + SEQ_HEADER_SIZE;
    return data;
}

size_t sequence_count(void *data) {
    return sequence_header(data)->count;
}

void *sequence_reallocate(void *data, size_t capacity) {
    struct sequence_header *header = sequence_header(data);
    size_t total_size = SEQ_HEADER_SIZE + header->element_size * capacity;
    header = realloc(header, total_size);
    header->capacity = capacity;
    data = (unsigned char *)header + SEQ_HEADER_SIZE;
    return data;
}

void *sequence_append(void *data, void *element) {
    struct sequence_header *header = sequence_header(data);
    printf("%d\n", (int)header->capacity);
    if (header->count == header->capacity) {
        size_t capacity = header->capacity == 0 ? 1 : header->capacity * 2;
        data = sequence_reallocate(data, capacity);
        header = sequence_header(data);
    }
    void *target = (unsigned char *)data + header->element_size * header->count;
    memcpy(target, element, header->element_size);
    ++header->count;
    return data;
}

void sequence_destroy(void *data) {
    struct sequence_header *header = sequence_header(data);
    free(header);
}