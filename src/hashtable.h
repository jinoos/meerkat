#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "hash.h"

typedef struct _hashtable
{
    int             power;
    void**          idx;
    pthread_mutex   *lock;
    int             isLockEnable;
} hashtable_t;

hashtable_t* hastable_init(int hash_power_size, pthread_mutex *lock);

#endif // _HASHTABLE_H_
