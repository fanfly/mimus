#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct variant {
    uint32_t type;
    unsigned char data[8];
};

char *load_string(FILE *file) {
    uint64_t length;
    fread(&length, sizeof(length), 1, file);
    char *string = malloc(length + 1);
    fread(string, 1, length, file);
    string[length] = '\0';
    return string;
}

void throw_typed_value(FILE *file, uint32_t type);

void throw_array(FILE *file) {
    uint32_t element_type;
    fread(&element_type, sizeof(element_type), 1, file);
    uint64_t length;
    fread(&length, sizeof(length), 1, file);
    for (int index = 0; index < length; ++index) {
        throw_typed_value(file, element_type);
    }
}

void throw_value(FILE *file) {
    uint32_t value_type;
    fread(&value_type, sizeof(value_type), 1, file);
    throw_typed_value(file, value_type);
}

void throw_typed_value(FILE *file, uint32_t type) {
    if (type != 8 && type != 9) {
        struct variant value;
        value.type = type;
        if (type == 0 || type == 1 || type == 7) {
            fread(&value.data, 1, 1, file);
        } else if (type == 2 || type == 3) {
            fread(&value.data, 1, 2, file);
        } else if (type == 4 || type == 5 || type == 6) {
            fread(&value.data, 1, 4, file);
        } else if (type == 10 || type == 11 || type == 12) {
            fread(&value.data, 1, 8, file);
        }
    } else if (type == 8) {
        char *value = load_string(file);
        free(value);
    } else {
        throw_array(file);
    }
}

void parse_gguf(FILE *file) {
    uint32_t magic;
    fread(&magic, sizeof(magic), 1, file);
    uint32_t version;
    fread(&version, sizeof(version), 1, file);
    uint64_t tensor_count;
    fread(&tensor_count, sizeof(tensor_count), 1, file);
    uint64_t key_count;
    fread(&key_count, sizeof(key_count), 1, file);
    printf("Number of keys: %" PRIu64 "\n", key_count);
    for (int key_index = 0; key_index < key_count; ++key_index) {
        char *key = load_string(file);
        printf("[%d] %s\n", key_index, key);
        throw_value(file);
        free(key);
    }
    printf("\n");
    printf("Number of tensors: %" PRIu64 "\n", tensor_count);
    for (int tensor_index = 0; tensor_index < tensor_count; ++tensor_index) {
        char *name = load_string(file);
        uint32_t rank;
        fread(&rank, sizeof(rank), 1, file);
        fseek(file, sizeof(uint64_t) * rank, SEEK_CUR);
        uint32_t element_type;
        fread(&element_type, sizeof(element_type), 1, file);
        uint64_t offset;
        fread(&offset, sizeof(offset), 1, file);
        printf("[%d] %s\n", tensor_index, name);
        free(name);
    }
    printf("\n");
}
