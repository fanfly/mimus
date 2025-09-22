#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "gguf.h"

// Returns a pointer to a duplicate of the string pointed by 'source'.
// The returned pointer must be freed later.
char *duplicate_string(const char *source) {
    int size = strlen(source) + 1;
    char *target = malloc(size);
    memcpy(target, source, size);
    return target;
}

struct arg_pack {
    char *model_path;
};

struct arg_pack *create_argument_pack() {
    struct arg_pack *pack = malloc(sizeof(struct arg_pack));
    pack->model_path = NULL;
    return pack;
}

void destroy_argument_pack(struct arg_pack *pack) {
    free(pack->model_path);
    free(pack);
}

bool parse_arg(char **args, int count, struct arg_pack *pack) {
    int index = 1;
    while (index < count) {
        if (strcmp(args[index], "-m") == 0) {
            ++index;
            if (index >= count) return false;
            free(pack->model_path);
            pack->model_path = duplicate_string(args[index]);
        }
        ++index;
    }
    return true;
}

void print_usage() {
    puts("Usage: mimus-tokenize -m MODEL_PATH");
}

int main(int argc, char **argv) {
    struct arg_pack *args = create_argument_pack();
    if (!parse_arg(argv, argc, args) || args->model_path == NULL) {
        print_usage();
        destroy_argument_pack(args);
        return 1;
    }
    const char *model_path = args->model_path;
    struct tokenizer_metadata *meta = create_tokenizer_metadata(model_path);
    printf("Vocabulary size: %" PRIu64 "\n", meta->vocab_size);
    for (int index = 0; index < meta->vocab_size; ++index) {
        printf("[%d] \"%s\"\n", index, meta->vocab[index]);
    }
    destroy_tokenizer_metadata(meta);
    destroy_argument_pack(args);
    return 0;
}
