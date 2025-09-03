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

int main(int argc, char **argv) {
    ArgumentPack *arg_pack = create_argument_pack();
    if (!parse(argv, argc, arg_pack) || arg_pack->model_path == NULL) {
        print_usage();
        destroy_argument_pack(arg_pack);
        return 1;
    }
    FILE *model_file = fopen(arg_pack->model_path, "rb");
    if (model_file == NULL) {
        printf("Failed: cannot be open %s\n", arg_pack->model_path);
        destroy_argument_pack(arg_pack);
        return 1;
    }
    fseek(model_file, 8, SEEK_SET);
    unsigned long long tensor_count = 0;
    if (fread(&tensor_count, 8, 1, model_file) != 1) {
        printf("Failed: cannot be get number of tensors\n");
        fclose(model_file);
        destroy_argument_pack(arg_pack);
        return 1;
    }
    printf("Number of tensors: %llu\n", tensor_count);
    fclose(model_file);
    puts("Unfortunately, tokenization is not currently supported.");
    destroy_argument_pack(arg_pack);
    return 1;
}
