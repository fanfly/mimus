#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns a pointer to a duplicate of the string pointed by 'source'.
// The returned pointer must be freed later.
char *duplicate_string(const char *source) {
    int size = strlen(source) + 1;
    char *target = malloc(size);
    memcpy(target, source, size);
    return target;
}

typedef struct {
    char *model_path;
} ArgumentPack;

ArgumentPack *create_argument_pack() {
    ArgumentPack *pack = malloc(sizeof(ArgumentPack));
    pack->model_path = NULL;
    return pack;
}

void destroy_argument_pack(ArgumentPack *pack) {
    free(pack->model_path);
    free(pack);
}

bool parse(char **args, int count, ArgumentPack *pack) {
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

int main(int arg_count, char **args) {
    ArgumentPack *pack = create_argument_pack();
    if (!parse(args, arg_count, pack) || pack->model_path == NULL) {
        print_usage();
        destroy_argument_pack(pack);
        return 1;
    }
    printf("Model path: %s\n", pack->model_path);
    puts("Unfortunately, tokenization is not supported now.");
    destroy_argument_pack(pack);
    return 1;
}
