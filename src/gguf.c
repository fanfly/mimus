#include <inttypes.h>
#include <stdbool.h>
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

size_t print_typed_value(uint32_t type, unsigned char *value) {
    if (type == 4) {
        printf("%" PRIu32, *(uint32_t *)value);
        return sizeof(uint32_t);
    } else if (type == 5) {
        printf("%" PRId32, *(int32_t *)value);
        return sizeof(int32_t);
    } else if (type == 6) {
        printf("%f", *(float *)value);
        return sizeof(float);
    } else if (type == 8) {
        putchar('"');
        uint64_t length = *(uint64_t *)value;
        char *string = (char *)(value + sizeof(length));
        for (int index = 0; index < length; ++index) {
            if (string[index] == '\n') {
                printf("\\n");
            } else if (string[index] == '"') {
                printf("\\\"");
            } else if ((unsigned char)string[index] >= (unsigned char)'\x80') {
                printf("\\x%x", (unsigned char)string[index]);
            } else {
                putchar(string[index]);
            }
        }
        putchar('"');
        return sizeof(uint64_t) + length;
    } else if (type == 9) {
        unsigned char *current = value;
        uint32_t element_type = *(uint32_t *)current;
        current += sizeof(element_type);
        uint64_t length = *(uint64_t *)current;
        current += sizeof(length);
        printf("{");
        size_t limit = 16;
        for (int index = 0; index < length && index < limit; ++index) {
            if (index > 0) {
                printf(", ");
            }
            size_t size = print_typed_value(element_type, current);
            current += size;
            if (index + 1 == limit && index + 1 != length) {
                printf(", ...");
            }
        }
        printf("} (%" PRIu64 " elements)", length);
        return current - (value + sizeof(uint32_t) + sizeof(uint64_t));
    } else {
        printf("(type-%" PRIu32 " value)", type);
        return 0;
    }
}

struct record {
    char *key;
    uint32_t type;
    void *value;
};

void load_record(FILE *file, struct record *record) {
    record->key = load_string(file);
    fread(&record->type, 1, sizeof(record->type), file);
    unsigned char *value;
    load_typed_value(file, record->type, &value);
    record->value = value;
}

void print_record(struct record *record) {
    printf("%s: ", record->key);
    if (strcmp(record->key, "tokenizer.chat_template") == 0) {
        printf("(chat template)");
    } else {
        print_typed_value(record->type, record->value);
    }
}

void clean_record(struct record *record) {
    free(record->key);
    free(record->value);
}

size_t get_record_length(struct record *record) {
    unsigned char *current = record->value;
    if (record->type == 8) {
        uint64_t length = *(uint64_t *)current;
        return length;
    }
    if (record->type == 9) {
        current += sizeof(uint32_t);
        uint64_t length = *(uint64_t *)current;
        return length;
    }
    return 0;
}

void print_gguf_metadata(const char *model_path) {
    FILE *file = fopen(model_path, "rb");
    if (file == NULL) {
        return;
    }
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
        struct record record;
        load_record(file, &record);
        printf("[%d] ", key_index);
        print_record(&record);
        puts("");
        clean_record(&record);
    }
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
    fclose(file);
}

struct tokenizer_metadata {
    uint64_t vocab_size;
    char **vocab;
};

struct tokenizer_metadata *create_tokenizer_metadata(const char *model_path) {
    FILE *file = fopen(model_path, "rb");
    if (file == NULL) {
        return NULL;
    }
    struct tokenizer_metadata *meta = malloc(sizeof(*meta));
    uint32_t magic;
    fread(&magic, sizeof(magic), 1, file);
    uint32_t version;
    fread(&version, sizeof(version), 1, file);
    uint64_t tensor_count;
    fread(&tensor_count, sizeof(tensor_count), 1, file);
    uint64_t key_count;
    fread(&key_count, sizeof(key_count), 1, file);
    for (int key_index = 0; key_index < key_count; ++key_index) {
        struct record record;
        load_record(file, &record);
        if (strcmp(record.key, "tokenizer.ggml.tokens") == 0) {
            meta->vocab_size = get_record_length(&record);
            unsigned char *current = record.value;
            current += sizeof(uint32_t);
            uint64_t length = *(uint64_t *)current;
            meta->vocab_size = length;
            current += sizeof(length);
            meta->vocab = malloc(sizeof(*(meta->vocab)) * meta->vocab_size);
            for (int index = 0; index < length; ++index) {
                uint64_t string_length = *(uint64_t *)current;
                current += sizeof(string_length);
                char *string = (char *)current;
                meta->vocab[index] = malloc(string_length + 1);
                memcpy(meta->vocab[index], string, string_length);
                meta->vocab[index][string_length] = '\0';
                current += string_length;
            }
        }
        clean_record(&record);
    }
    fclose(file);
    return meta;
}

void destroy_tokenizer_metadata(struct tokenizer_metadata *meta) {
    for (int index = 0; index < meta->vocab_size; ++index) {
        free(meta->vocab[index]);
    }
    free(meta->vocab);
    free(meta);
}
