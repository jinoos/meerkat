#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "logging.h"
#include "service.h"

service_t* service_init(char *name)
{
    service_t *service = calloc(sizeof(service_t), 1);
    service->name = name;
    service->len  = strlen(name);
    return service;
}

service_t** service_index_init(int power)
{
    service_t** idx = calloc(hashsize(power), sizeof(void *));

    if(idx == NULL)
    {
        log_err(__FILE__, __LINE__, "Cannot init service index.");
        return NULL;
    }

    return idx;
}

service_t* service_find2(service_t** idx, int power, char *svc_name, size_t svc_len)
{
    if(idx == NULL || svc_name == NULL || svc_len == 0)
    {
        return NULL;
    }

    uint32_t hv = hash(svc_name, svc_len, 0);

    service_t *service = idx[hv & hashmask(power)];

    while(service != NULL)
    {
        if(strncmp(service->name, svc_name, svc_len) == 0)
        {
            return service;
        }

        service = service->all_next;
    }
    
    return NULL;
}

service_t* service_find(service_t** idx, int power, char *svc_name)
{
    return service_find2(idx, power, svc_name, strlen(svc_name));
}

service_t* service_add(service_t** idx, int power, service_t* svc)
{
    if(idx == NULL || svc == NULL)
    {
        return NULL;
    }

    uint32_t hv = hash(svc->name, strlen(svc->name), 0);

    service_t *service = idx[hv & hashmask(power)];

    if(service == NULL)
    {
        idx[hv & hashmask(power)] = svc;
        return svc;
    }

    service_t *oservice = NULL;

    while(service != NULL)
    {
        if(strncmp(service->name, svc->name, svc->len) == 0)
        {
            return NULL;
        }

        oservice = service;
        service = service->next;
    }

    oservice->next = svc;

    return svc;
}
