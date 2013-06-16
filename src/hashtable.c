#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

hashtable_t* hashtable_init(int hash_power_size, pthread_mutex_t *lock, void (*cb_data_free)(void *))
{
    hashtable_t *table = calloc(sizeof(hashtable_t), 1);

    table->idx = calloc(hashsize(hash_power_size), sizeof(void *));

    if(table->idx == NULL)
    {
        free(table);
        return NULL;
    }

    table->power = hash_power_size;

    table->lock = lock;
    table->callback_data_free = cb_data_free;

    return table;
}

hashtable_data_t* hashtable_data_init(char *key, void *data)
{
    if(key == NULL)
        return NULL;

    HASH_KEY_LEN len = strlen(key);

    if(len == 0)
        return NULL;

    hashtable_data_t *hdata = calloc(sizeof(hashtable_data_t), 1);

    if(hdata == NULL)
        return NULL;

    hdata->key = key;
    hdata->len = len;
    hdata->data = data;

    return hdata;
}

void hashtable_data_free(hashtable_t *table, hashtable_data_t *hdata)
{
    if(hdata == NULL)
        return;

    if(table->callback_data_free != NULL)
    {
        table->callback_data_free(hdata->data);
    }

    free(hdata);

    return;
}

hashtable_data_t* hashtable_get(hashtable_t *table, const char *key)
{
    if(table == NULL || key == NULL)
        return NULL;

    return hashtable_get2(table, key, strlen(key));
}

hashtable_data_t* hashtable_get2(hashtable_t *table, const char *key, const HASH_KEY_LEN len)
{
    if(table->lock != NULL)
    {
        pthread_mutex_lock(table->lock);
    }

    HASH_VAL hval = hash(key, (size_t)len, 0);

    hashtable_data_t *data = table->idx[hval & hashmask(table->power)];

    while(data != NULL)
    {
        if(strncmp(data->key, key, (size_t)len) == 0)
        {
            break;
        }
        data = data->next;
    }

    if(table->lock != NULL)
    {
        pthread_mutex_unlock(table->lock);
    }

    return data;
}

hashtable_data_t* hashtable_set(hashtable_t *table, hashtable_data_t *data)
{
    if(table == NULL || data == NULL)
        return NULL;

    if(table->lock != NULL)
    {
        pthread_mutex_lock(table->lock);
    }

    HASH_VAL hval = hash(data->key, data->len, 0);
    hashtable_data_t *idx = table->idx[hval & hashmask(table->power)];
    hashtable_data_t *prev = NULL;

    while(idx != NULL)
    {
        if(strncmp(idx->key, data->key, data->len) == 0)
        {
            if(table->lock != NULL)
            {
                pthread_mutex_unlock(table->lock);
            }

            return NULL;
        }

        prev = idx;
        idx = idx->next;
    }

    if(prev == NULL)
    {
        table->idx[hval & hashmask(table->power)] = data;
    }else
    {
        prev->next = data;
    }

    table->data_count++;

    if(table->lock != NULL)
    {
        pthread_mutex_unlock(table->lock);
    }

    return data;
}

void hashtable_del(hashtable_t *table, const char *key)
{
    if(table == NULL || key == NULL)
        return;

    return hashtable_del2(table, key, strlen(key));
}

void hashtable_del2(hashtable_t *table, const char *key, const HASH_KEY_LEN len)
{
    if(table == NULL || key == NULL)
        return;

    if(table->lock != NULL)
    {
        pthread_mutex_lock(table->lock);
    }

    HASH_VAL hval = hash(key, (size_t)len, 0);
    hashtable_data_t *idx = table->idx[hval & hashmask(table->power)];
    hashtable_data_t *prev = NULL;

    while(idx != NULL)
    {
        if(strncmp(idx->key, key, (size_t)len) == 0)
        {
            // first item.
            if(prev == NULL)
            {
                table->idx[hval & hashmask(table->power)] = idx->next;
            }else
            {
                prev->next = idx->next;
            }

            table->data_count--;
            idx->next = NULL;

            if(table->lock != NULL)
            {
                pthread_mutex_unlock(table->lock);
            }

            hashtable_data_free(table, idx);
            return;
        }

        prev = idx;
        idx = idx->next;
    }

    if(table->lock != NULL)
    {
        pthread_mutex_unlock(table->lock);
    }

    return;
}

