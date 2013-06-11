#include <stdlib.h>
#include <unistd.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "logging.h"
#include "client.h"

void client_free(client_t *client)
{
    if(client == NULL)
        return;

    if(client->bufev != NULL)
    {
        bufferevent_free(client->bufev);
    }

    if(client->buf != NULL)
    {
        evbuffer_free(client->buf);
    }

    if(client->socket != 0)
    {
        close(client->socket);
    }

    free(client);
    return;
}
