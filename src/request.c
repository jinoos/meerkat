#include "request.h"

request_t* request_init(int flag, char *line)
{
    request_t *req = calloc(sizeof(request_t), 1);
    req->flag = flag;
    req->cmd  = line;
    return req;
}

request2_t* request2_init(

void request_free(request_t *req)
{
    if(req == NULL)
        return;

    if(req->service != NULL)
    {
        free(req->service);
    }

    if(req->key != NULL)
    {
        free(req->key);
    }

    free(req);
    req = NULL;

    return;
}

