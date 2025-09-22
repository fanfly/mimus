#ifndef GGUF_H
#define GGUF_H

#include <stdint.h>

struct tokenizer_metadata {
    uint64_t vocab_size;
    char **vocab;
};

void print_gguf_metadata(const char *model_path);
struct tokenizer_metadata *create_tokenizer_metadata(const char *model_path);
void destroy_tokenizer_metadata(struct tokenizer_metadata *meta);

#endif // GGUF_H
