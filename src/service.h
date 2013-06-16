#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "meerkat.h"
#include "hashtable.h"

#define SERVICE_POWER_FACTOR    8

void service_free(void *arg);

hashtable_t* service_ht_init();
service_t* service_set(hashtable_t *table, char *name, service_mothod method, uint16_t num_all_nodes, service_node_t *node_arr[]);
service_t* service_get(hashtable_t *table, char *name);
void service_del(hashtable_t *table);

#endif // _SERVICE_H_
