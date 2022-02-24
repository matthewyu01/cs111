#include "hash-table-base.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <pthread.h>

struct list_entry
{
    const char *key;
    uint32_t value;
    SLIST_ENTRY(list_entry)
    pointers;
};

SLIST_HEAD(list_head, list_entry);

pthread_mutex_t my_mutexes[HASH_TABLE_CAPACITY];

struct hash_table_entry
{
    struct list_head list_head;
};

struct hash_table_v2
{
    struct hash_table_entry entries[HASH_TABLE_CAPACITY];
};

struct hash_table_v2 *hash_table_v2_create()
{
    int error_check;
    struct hash_table_v2 *hash_table = calloc(1, sizeof(struct hash_table_v2));
    assert(hash_table != NULL);
    for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i)
    {
        struct hash_table_entry *entry = &hash_table->entries[i];
        SLIST_INIT(&entry->list_head);
        error_check = pthread_mutex_init(&(my_mutexes[i]), NULL);
        if (error_check != 0)
        {
            exit(error_check);
        }
    }
    return hash_table;
}

static struct hash_table_entry *get_hash_table_entry(struct hash_table_v2 *hash_table,
                                                     const char *key)
{
    assert(key != NULL);
    uint32_t index = bernstein_hash(key) % HASH_TABLE_CAPACITY;
    struct hash_table_entry *entry = &hash_table->entries[index];
    return entry;
}

static struct list_entry *get_list_entry(struct hash_table_v2 *hash_table,
                                         const char *key,
                                         struct list_head *list_head)
{
    assert(key != NULL);

    struct list_entry *entry = NULL;

    SLIST_FOREACH(entry, list_head, pointers)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry;
        }
    }
    return NULL;
}

bool hash_table_v2_contains(struct hash_table_v2 *hash_table,
                            const char *key)
{
    struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
    struct list_head *list_head = &hash_table_entry->list_head;
    struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
    return list_entry != NULL;
}

void hash_table_v2_add_entry(struct hash_table_v2 *hash_table,
                             const char *key,
                             uint32_t value)
{
    struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
    struct list_head *list_head = &hash_table_entry->list_head;
    struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);

    uint32_t index = bernstein_hash(key) % HASH_TABLE_CAPACITY;
    int error_check = pthread_mutex_lock(&(my_mutexes[index]));
    if (error_check != 0)
    {
        exit(error_check);
    }

    /* Update the value if it already exists */
    if (list_entry != NULL)
    {
        list_entry->value = value;
        return;
    }

    list_entry = calloc(1, sizeof(struct list_entry));
    list_entry->key = key;
    list_entry->value = value;
    SLIST_INSERT_HEAD(list_head, list_entry, pointers); // dangerous

    error_check = pthread_mutex_unlock(&(my_mutexes[index]));
    if (error_check != 0)
    {
        exit(error_check);
    }
}

uint32_t hash_table_v2_get_value(struct hash_table_v2 *hash_table,
                                 const char *key)
{
    struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
    struct list_head *list_head = &hash_table_entry->list_head;
    struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
    assert(list_entry != NULL);
    return list_entry->value;
}

void hash_table_v2_destroy(struct hash_table_v2 *hash_table)
{
    int error_check = 0;

    for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i)
    {
        struct hash_table_entry *entry = &hash_table->entries[i];
        struct list_head *list_head = &entry->list_head;
        struct list_entry *list_entry = NULL;
        while (!SLIST_EMPTY(list_head))
        {
            list_entry = SLIST_FIRST(list_head);
            SLIST_REMOVE_HEAD(list_head, pointers);
            free(list_entry);
            error_check = pthread_mutex_destroy(&(my_mutexes[i]));
            if (error_check)
            { // frees memory
                exit(error_check);
            }
        }
    }
    free(hash_table);
}
