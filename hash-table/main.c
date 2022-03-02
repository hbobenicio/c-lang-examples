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

#define EXIT_IF_TRUE(condition, msg) \
    do { \
        if (condition) { \
            perror("error: fatal: " msg); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// API can be improved if this could be defined by the user
struct hash_table_entry {
    /**
     * @brief The entry key. NULL is not a valid entry, because we'll use it as a flag to detect unused entries.
     * 
     */
    char* key;
    int value;
};

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

uint32_t hash_fnv_1a(const char* key, size_t key_len);

void hash_table_init(struct hash_table* ht, size_t size)
{
    ht->entries = calloc(size, sizeof(struct hash_table_entry));
    EXIT_IF_TRUE(ht->entries == NULL, "failed to allocate memory for hash table");

    ht->capacity = size;
    ht->count = 0;
}

void hash_table_free(struct hash_table* ht)
{
    for (size_t i = 0; i < ht->capacity; i++) {
        if (ht->entries[i].key) {
            free(ht->entries[i].key);
        }
    }
    free(ht->entries);
    ht->entries = NULL;
    ht->capacity = ht->count = 0;
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
    size_t index = hash_fnv_1a(key, strlen(key)) % ht->capacity;
    
    if (ht->entries[index].key == NULL) {
        ht->entries[index] = hash_table_entry_new(key, value);
        return;
    }

    struct hash_table_entry* prev;
    while (entry != NULL) {
        // Same key, update it's value
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        prev = entry;
        entry = entry->next;
    }

    // Collision. Append to the linked list
    prev->next = hash_table_entry_new(key, value);
}

int* hash_table_get(struct hash_table* ht, const char* key)
{
    size_t index = hash_fnv_1a(key, strlen(key), ht->capacity);
    struct hash_table_entry* entry = ht->entries[index];
    if (entry == NULL) {
        return NULL;
    }

    return &entry->value;
}

void hash_table_fdump(struct hash_table* ht, FILE* file)
{
    for (size_t i = 0; i < ht->capacity; i++) {
        struct hash_table_entry* entry = ht->entries[i];
        if (entry) {
            fprintf(file, "[%zu]: ", i);
            while (entry != NULL) {
                fprintf(file, "%s => %d, ", entry->key, entry->value);
                entry = entry->next;
            }
            fputc('\n', file);
        }
    }
}

int main()
{
    puts("=== Hash Table implementation ===");
    struct hash_table ht;
    // hash_table_init(&ht, 1e9 + 9);
    hash_table_init(&ht, 10);

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
