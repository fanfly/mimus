#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char *load_string(FILE *file) {
    uint64_t length;
    fread(&length, sizeof(length), 1, file);
    char *string = malloc(length + 1);
    fread(string, 1, length, file);
    string[length] = '\0';
    return string;
}

size_t size_of_scalar_type(uint32_t type) {
    if (type == 0 || type == 1 || type == 7) {
        return 1;
    }
    if (type == 2 || type == 3) {
        return 2;
    }
    if (type == 4 || type == 5 || type == 6) {
        return 4;
    }
    if (type == 10 || type == 11 || type == 12) {
        return 8;
    }
    return 0;
}

size_t load_typed_value(FILE *file, uint32_t type, unsigned char **value) {
    if (type == 8) {
        uint64_t length;
        fread(&length, sizeof(length), 1, file);
        *value = malloc(sizeof(length) + length);
        memcpy(*value, &length, sizeof(length));
        fread(*value + sizeof(length), 1, length, file);
        return sizeof(length) + length;
    }
    if (type == 9) {
        uint32_t element_type;
        fread(&element_type, sizeof(element_type), 1, file);
        uint64_t length;
        fread(&length, sizeof(length), 1, file);
        size_t size_sum = 0;
        unsigned char **elements = malloc(sizeof(unsigned char *) * length);
        size_t *sizes = malloc(sizeof(size_t) * length);
        for (int index = 0; index < length; ++index) {
            sizes[index] = load_typed_value(
                file, element_type, &elements[index]
            );
            size_sum += sizes[index];
        }
        *value = malloc(sizeof(element_type) + sizeof(length) + size_sum);
        unsigned char *target = *value;
        memcpy(target, &element_type, sizeof(element_type));
        target += sizeof(element_type);
        memcpy(target, &length, sizeof(length));
        target += sizeof(length);
        for (int index = 0; index < length; ++index) {
            memcpy(target, elements[index], sizes[index]);
            target += sizes[index];
            free(elements[index]);
        }
        free(elements);
        free(sizes);
        return sizeof(element_type) + sizeof(length) + size_sum;
    }
    size_t value_size = size_of_scalar_type(type);
    *value = malloc(value_size);
    fread(*value, 1, value_size, file);
    return value_size;
}

void print_typed_value(uint32_t type, unsigned char *value) {
    if (type == 4) {
        printf("%" PRIu32, *(uint32_t *)value);
    } else if (type == 5) {
        printf("%" PRId32, *(int32_t *)value);
    } else if (type == 6) {
        printf("%f", *(float *)value);
    } else if (type == 8) {
        putchar('"');
        uint64_t length = *(uint64_t *)value;
        char *string = (char *)(value + sizeof(length));
        for (int index = 0; index < length; ++index) {
            if (string[index] == '\n') {
                printf("\\n");
            } else if (string[index] == '"') {
                printf("\\\"");
            } else {
                putchar(string[index]);
            }
        }
        putchar('"');
    } else if (type == 9) {
        uint32_t element_type = *(uint32_t *)value;
        uint64_t length = *(uint64_t *)(value + sizeof(element_type));
        printf(
            "[type-%" PRIu32 " array of length %" PRIu64 "]",
            element_type,
            length
        );
    } else {
        printf("[type-%" PRIu32 " value]", type);
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
        printf("[%d] %s: ", key_index, key);
        uint32_t type;
        fread(&type, 1, sizeof(type), file);
        unsigned char *value;
        load_typed_value(file, type, &value);
        print_typed_value(type, value);
        puts("");
        free(value);
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
