#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "logging.h"
#include "service.h"
#include "service.h"

void service_free(void *arg)
{
    service_t *service = (service_t*) arg;
    if(service == NULL);
}

hashtable_t* service_ht_init()
{
    hashtable_t *table = hashtable_init(SERVICE_POWER_FACTOR, NULL, service_free);

    if(table == NULL)
        return NULL;
}

service_t* service_set(hashtable_t *table, char *name, service_mothod method, uint16_t num_all_nodes, service_node_t *node_arr[])
{
    hashtable_data_t *data = hashtable_get(table, name);

    if(data != NULL)
        return NULL;

    service_t *service = calloc(sizeof(service_t), 1);
    if(service == NULL)
        return NULL;

    service->name           = name;
    service->len            = strlen(name);
    service->method         = method;
    service->num_all_nodes  = num_all_nodes;

    int i;
    service_node_t *snode, *sprev;

    for(i = 0; i < num_all_nodes; i++)
    {
        snode = node_arr[i];

        if(service->all_next == NULL)
        {
            service->all_next = snode;
        }else
        {
            sprev->all_next = snode;
        }

        sprev = snode;
    }

    data = hashtable_data_init(name, (void *) service);

    if(data == NULL)
    {
        service_free(service);
        return NULL;
    }

    hashtable_set(table, data);

    return service;
}

service_t* service_get(hashtable_t *table, char *name)
{
    if(table == NULL)
    {
        return NULL;
    }

    if(name == NULL)
    {
        return NULL;
    }

    hashtable_data_t *data = hashtable_get(table, name);

    if(data == NULL)
    {
        return NULL;
    }

    return (service_t*) data->data;
}

void service_del(hashtable_t *table)
{
    // need to implement
}

