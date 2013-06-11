#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

hashtable_t* hastable_init(int hash_power_size, pthread_mutex *lock)
{
    hashtable_t *table = calloc(sizeof(hashtable_t), 1);
    if(hash == NULL)
    {
        return NULL;
    }

    table->idx = calloc(hashsize(hash_power_size), sizeof(void *));
    if(table->idx == NULL)
    {
        free(table);
        return NULL;
    }

    table->power = hash_power_size;

    table->lock = lock;

    return table;
}

void* hashtable_find_idx(hashtable_t *table, uint32_t hash)
{
    if(table->lock)
    {
        pthread_mutex_lock(table->lock);
    }



    if(table->lock)
    {
        pthread_mutex_unlock(table->lock);
    }
}
