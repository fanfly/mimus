#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    puts("Usage: tokenize [-m MODEL_PATH]");
}

void parse_gguf(FILE *file);

int main(int argc, char **argv) {
    struct arg_pack *args = create_argument_pack();
    if (!parse_arg(argv, argc, args) || args->model_path == NULL) {
        print_usage();
        destroy_argument_pack(args);
        return 1;
    }
    FILE *model_file = fopen(args->model_path, "rb");
    if (model_file == NULL) {
        printf("Failed: cannot be open %s\n", args->model_path);
        destroy_argument_pack(args);
        return 1;
    }
    parse_gguf(model_file);
    fclose(model_file);
    puts("Unfortunately, tokenization is not currently supported.");
    destroy_argument_pack(args);
    return 1;
}
