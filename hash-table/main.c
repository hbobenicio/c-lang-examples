#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// API could be improved if this could be controlled by the user
struct hash_table_entry {
    char* key;
    int value;

    /**
     * @brief Singly Linked List used in case of hash collitions.
     */
    struct hash_table_entry* next;
};

struct hash_table {
    /**
     * @brief The table of entries itself. It's an array of pointers to entries.
     * 
     * This could be a dynamic array which grows by a factor when insertin items,
     * but this improvement will not be done atm. For now, we only use a fixed
     * amount of items.
     */
    struct hash_table_entry** entries;
    /**
     * @brief The capacity of the entries. Stores how much space has been heap-allocated.
     */
    size_t entries_capacity;
    /**
     * @brief Counter of items that had been stored in the table.
     */
    size_t count;
};

void hash_table_init(struct hash_table* ht, size_t size);
void hash_table_free(struct hash_table* ht);
size_t hash_char_array(const char* s, size_t s_len, size_t max);
struct hash_table_entry* hash_table_entry_new(const char* key, int value);
void hash_table_entry_free(struct hash_table_entry* entry);
void hash_table_put(struct hash_table* ht, const char* key, int value);
int* hash_table_get(struct hash_table* ht, const char* key);
void hash_table_fdump(struct hash_table* ht, FILE* file);

void hash_table_init(struct hash_table* ht, size_t size)
{
    ht->entries = calloc(size, sizeof(struct hash_table_entry*));
    if (ht->entries == NULL) {
        perror("error: could not heap-allocate memory for hash table");
        exit(EXIT_FAILURE);
    }
    ht->entries_capacity = size;
    ht->count = 0;
}

void hash_table_free(struct hash_table* ht)
{
    for (size_t i = 0; i < ht->entries_capacity; i++) {
        struct hash_table_entry* entry = ht->entries[i];
        while(entry != NULL) {
            struct hash_table_entry* next = entry->next;
            hash_table_entry_free(entry);
            entry = next;
        }
    }
    free(ht->entries);
    ht->entries = NULL;
    ht->entries_capacity = ht->count = 0;
}

/**
 * @brief Calculates the hash of a array of chars.
 * 
 * Borrowed from https://cp-algorithms.com/string/string-hashing.html
 * 
 * @param s char array to be hashed
 * @param s_len lenght of the array
 * @return size_t hash number
 */
size_t hash_char_array(const char* s, size_t s_len, size_t max)
{
    size_t h = 0;

    // It is reasonable to make a prime number roughly equal to the number of characters in the input alphabet.
    // For example, if the input is composed of only lowercase letters of the English alphabet, is a good choice.
    // If the input may contain both uppercase and lowercase letters, then is a possible choice
    static const size_t p = 53;

    // This is a large number, but still small enough so that we can perform multiplication of two values using 64-bit integers
    // static const size_t m = 1e9 + 9;

    for (size_t i = 0; i < s_len; i++) {
        // Precomputing the powers of might give a performance boost.
        h = (h + s[i] * (size_t) pow((double)p, (double)i)) % max;
    }

    return h;
}

struct hash_table_entry* hash_table_entry_new(const char* key, int value)
{
    struct hash_table_entry* new_entry = malloc(sizeof(struct hash_table_entry));
    if (new_entry == NULL) {
        perror("error: could not heap-allocate memory for an new hash table entry");
        exit(EXIT_FAILURE);
    }

    size_t key_len = strlen(key);
    size_t key_capacity = key_len + 1;
    new_entry->key = malloc(key_capacity * sizeof(new_entry->key));
    strncpy(new_entry->key, key, key_capacity);

    new_entry->value = value;
    new_entry->next = NULL;
    
    return new_entry;
}

void hash_table_entry_free(struct hash_table_entry* entry)
{
    free(entry->key);
    entry->key = NULL;

    free(entry);
}

void hash_table_put(struct hash_table* ht, const char* key, int value)
{
    size_t index = hash_char_array(key, strlen(key), ht->entries_capacity);
    
    struct hash_table_entry* entry = ht->entries[index];
    if (entry == NULL) {
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
    size_t index = hash_char_array(key, strlen(key), ht->entries_capacity);
    struct hash_table_entry* entry = ht->entries[index];
    if (entry == NULL) {
        return NULL;
    }

    return &entry->value;
}

void hash_table_fdump(struct hash_table* ht, FILE* file)
{
    for (size_t i = 0; i < ht->entries_capacity; i++) {
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
