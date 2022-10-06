/**
 * @brief A simple implementation of a Hash Table using Open Addressing for collisions
 * @author Hugo Benicio <hbobenicio@gmail.com>
 * 
 * Many thanks to Bob Nystrom for this:
 * https://craftinginterpreters.com/hash-tables.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#define EXIT_IF(condition, msg) \
    do { \
        if (condition) { \
            perror("error: " msg); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

#define HASH_TABLE_VALUE_TYPE int

// NOTE API can be improved if this could be defined by the user
struct hash_table_entry {
    /**
     * @brief The entry key. NULL is not a valid entry, because we'll use it as a flag to detect unused entries.
     */
    char* key;

    //NOTE API can be improved if this could be defined by the user
    HASH_TABLE_VALUE_TYPE value;
};

/**
 * @brief The Hash Table data structure.
 */
struct hash_table {
    /**
     * @brief The table of entries itself. It's just a dynamic array of entries.
     */
    struct hash_table_entry* entries;
    /**
     * @brief The capacity of the entries array. Indicates how much space has been heap-allocated for it.
     */
    size_t capacity;
    /**
     * @brief Counter of items that had been stored in the table.
     */
    size_t count;
};

void hash_table_init(struct hash_table* ht, size_t size);
void hash_table_free(struct hash_table* ht);
void hash_table_put(struct hash_table* ht, const char* key, int value);
int* hash_table_get(struct hash_table* ht, const char* key);
void hash_table_fdump(struct hash_table* ht, FILE* file);

void hash_table_entry_free(struct hash_table_entry* entry);

uint32_t hash_fnv_1a_32(const char* key, size_t key_len);

void hash_table_init(struct hash_table* ht, size_t size)
{
    ht->entries = calloc(size, sizeof(struct hash_table_entry));
    EXIT_IF(ht->entries == NULL, "failed to allocate memory for hash table");

    ht->capacity = size;
    ht->count = 0;
}

void hash_table_free(struct hash_table* ht)
{
    for (size_t i = 0; i < ht->capacity; i++) {
        hash_table_entry_free(&ht->entries[i]);
    }
    free(ht->entries);
    ht->entries = NULL;
    ht->capacity = 0;
    ht->count = 0;
}

/**
 * @brief Calculates the hash of a array of chars.
 */
uint32_t hash_fnv_1a_32(const char* key, size_t key_len)
{
#define FNV_1A_32_OFFSET_BASIS 2166136261u
#define FNV_1A_32_PRIME 16777619

    uint32_t hash = FNV_1A_32_OFFSET_BASIS;

    for (size_t i = 0; i < key_len; i++) {
        hash ^= (uint8_t) key[i];
        hash *= FNV_1A_32_PRIME;
    }

    return hash;

#undef FNV_1A_32_PRIME
#undef FNV_1A_32_OFFSET_BASIS
}

void hash_table_entry_free(struct hash_table_entry* entry)
{
    if (entry->key) {
        free(entry->key);
        entry->key = NULL;
    }
}

void hash_table_put(struct hash_table* ht, const char* key, int value)
{
    assert(key != NULL && "NULL keys not allowed here because we use it to check if entry is empty");

    //TODO this can be avoided if informed by the user
    size_t key_len = strlen(key);
    size_t index = hash_fnv_1a_32(key, key_len) % ht->capacity;

    //TODO calculate load factor
    //TODO if load factor >= some threshold, then reallocate the entries
    {
        struct hash_table_entry* entry = &ht->entries[index];
        if (entry->key == NULL) {
            entry->key = calloc(key_len + 1, sizeof(char));
            strcpy(entry->key, key);
            entry->value = value;
            ht->count++;
            return;
        }
    }

    size_t i = index;
    size_t count = 0;
    while (ht->entries[i].key != NULL && strcmp(ht->entries[i].key, key) != 0) {
        assert(++count < ht->capacity);
        i = (i + 1) % ht->capacity;
    }
    struct hash_table_entry* entry = &ht->entries[i];

    if (entry->key == NULL) {
        entry->key = calloc(key_len + 1, sizeof(char));
        ht->count++;
    }
    strcpy(entry->key, key);
    entry->value = value;
}

int* hash_table_get(struct hash_table* ht, const char* key)
{
    size_t index = hash_fnv_1a_32(key, strlen(key)) % ht->capacity;

    struct hash_table_entry* entry = &ht->entries[index];
    if (entry->key == NULL) {
        return NULL;
    }

    size_t count = 0;
    while (strcmp(entry->key, key) != 0 && entry->key != NULL) {
        if (++count < ht->count) {
            return NULL;
        }
        index = (index + 1) % ht->capacity;
        entry = &ht->entries[index];
    }
    if (entry->key == NULL) {
        return NULL;
    }

    return &entry->value;
}

void hash_table_fdump(struct hash_table* ht, FILE* file)
{
    for (size_t i = 0; i < ht->capacity; i++) {
        struct hash_table_entry* entry = &ht->entries[i];
        if (entry->key) {
            fprintf(file, "[%zu]: %s => %d\n", i, entry->key, entry->value);
        }
    }
}

int main()
{
    puts("=== Hash Table implementation ===");
    struct hash_table ht;
    hash_table_init(&ht, 8);

    fputs("Inserting 'hugo' => 2\n", stderr);
    hash_table_put(&ht, "hugo", 2);
    hash_table_fdump(&ht, stderr);
    fputs("----\n", stderr);

    fputs("Inserting 'benicio' => 4\n", stderr);
    hash_table_put(&ht, "benicio", 4);
    hash_table_fdump(&ht, stderr);
    fputs("----\n", stderr);

    fputs("Inserting 'miranda' => 8\n", stderr);
    hash_table_put(&ht, "miranda", 8);
    hash_table_fdump(&ht, stderr);
    fputs("----\n", stderr);
    
    fputs("Inserting 'de' => 16\n", stderr);
    hash_table_put(&ht, "de", 16);
    hash_table_fdump(&ht, stderr);
    fputs("----\n", stderr);

    fputs("Inserting 'oliveira' => 24\n", stderr);
    hash_table_put(&ht, "oliveira", 24);
    hash_table_fdump(&ht, stderr);
    fputs("----\n", stderr);

    printf("hugo => %d\n", *hash_table_get(&ht, "hugo"));

    hash_table_free(&ht);
    return 0;
}
