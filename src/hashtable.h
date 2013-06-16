#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <pthread.h>

#include "hash.h"

typedef struct _hashtable_data hashtable_data_t;
typedef struct _hashtable_data
{
    char                *key;
    HASH_KEY_LEN        len;
    void                *data;
    hashtable_data_t    *next;
} hashtable_data_t;

typedef struct _hashtable
{
    int                 power;
    hashtable_data_t    **idx;
    uint64_t            data_count;
    pthread_mutex_t     *lock;
    void                (*callback_data_free)(void *);
} hashtable_t;

hashtable_t*        hashtable_init(int hash_power_size, pthread_mutex_t *lock, void (*cb_data_free)(void *));
hashtable_data_t*   hashtable_data_init(char *key, void *data);

void                hashtable_data_free(hashtable_t *table, hashtable_data_t *hdata);

hashtable_data_t*   hashtable_get(hashtable_t *table, const char *key);
hashtable_data_t*   hashtable_get2(hashtable_t *table, const char *key, const HASH_KEY_LEN len);

hashtable_data_t*   hashtable_set(hashtable_t *table, hashtable_data_t *data);

void                hashtable_del(hashtable_t *table, const char *key);
void                hashtable_del2(hashtable_t *table, const char *key, const HASH_KEY_LEN len);

#endif // _HASHTABLE_H_
