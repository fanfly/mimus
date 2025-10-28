#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <stddef.h>

struct sequence;

/*
 * Creates a sequence.
 *
 * Args:
 *     size: The size in bytes of an element in the sequence.
 *
 * Note:
 *     The caller needs to cleanup later by calling `sequence_destroy`.
 */
struct sequence *sequence_create(size_t size);

/*
 * Returns the number of elements in the sequence.
 */
size_t sequence_count(struct sequence *sequence);

/*
 * Returns a pointer to the element with the specified index.
 */
void *sequence_get(struct sequence *sequence, size_t index);

/*
 * Appends an element to the end of the sequence.
 */
void sequence_append(struct sequence *sequence, void *element);

/*
 * Destroys a sequence.
 */
void sequence_destroy(struct sequence *sequence);

#endif // SEQUENCE_H_
