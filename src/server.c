#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

#include "logging.h"
#include "meerkat.h"
#include "server.h"
#include "service.h"
#include "client.h"
#include "message.h"

//server_t* server_init(struct sockaddr_in *listen_addr)
server_t* server_init(int port)
{
    log_debug(__FILE__, __LINE__, "Open server socket. Port : %d", port);

    server_t *server    = calloc(sizeof(server_t), 1);

    // set address
    server->addr        = calloc(sizeof(struct sockaddr_in), 1);

    server->addr->sin_family         = AF_INET;
    server->addr->sin_addr.s_addr    = INADDR_ANY;
    server->addr->sin_port           = htons(port);

    server->evbase      = event_base_new();

    if(!server->evbase)
    {
        log_err(__FILE__, __LINE__, "Cannot init event base.");
        return NULL;
    }

    server->evlistener  = evconnlistener_new_bind(server->evbase, server_event_accept, (void *)server, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)server->addr, sizeof(struct sockaddr_in));

    if(!server->evlistener)
    {
        log_err(__FILE__, __LINE__, "Cannot init event listener. error : %s", evutil_socket_error_to_string(errno));
        return NULL;
    }

    evconnlistener_set_error_cb(server->evlistener, server_event_error2);

    return server;
}

void server_free(server_t *server)
{
    if(server == NULL)
        return;

    if(server->addr)
    {
        free(server->addr);
    }

    if(server->socket)
    {
        close(server->socket);
    }

    if(server->evbase)
    {
        event_base_free(server->evbase);
    }

    if(server->evlistener)
    {
        evconnlistener_free(server->evlistener);
    }

    if(server->evsignal)
    {
        event_base_free(server->evsignal);
    }

    free(server);

    return;
}


struct event_base* server_event_base_init()
{
    struct event_config *config = event_config_new();
    event_config_avoid_method(config, "select");
    event_config_avoid_method(config, "poll");
    struct event_base *base = event_base_new_with_config(config);
    event_config_free(config);

    if(base == NULL)
    {
        log_err(__FILE__, __LINE__, "Cannot initialize event_base. error : %s", evutil_socket_error_to_string(errno));
        return NULL;
    }

    return base;
}

void server_event_run(server_t *server)
{
    log_info(__FILE__, __LINE__, "Starting server to accept client requests.");

    event_base_dispatch(server->evbase);

    evconnlistener_free(server->evlistener);
    event_base_free(server->evbase);

    log_info(__FILE__, __LINE__, "Stop server to accept client requests.");
    return;
} 

void server_event_accept(struct evconnlistener *listener, evutil_socket_t client_socket, struct sockaddr *sa, int socklen, void *arg)
{

    server_t *server = (server_t *)arg;
    client_t *client = calloc(sizeof(client_t), 1);
    client->addr     = (struct sockaddr_in *)sa;

    client->bufev = bufferevent_socket_new(server->evbase, client_socket, BEV_OPT_CLOSE_ON_FREE);

    if(!client->bufev)
    {
        log_err(__FILE__, __LINE__, "Error constructing bufferevent.");
    }

    client->server  = server;
    client->socket  = client_socket;

    client->buf     = evbuffer_new();
    client->status  = REQ_PKT_NONE;

    bufferevent_setcb(client->bufev, server_event_read, server_event_write, server_event_error, client);
    bufferevent_enable(client->bufev, EV_READ);

    server->cur_cons++;
    server->all_cons++;

    log_info(__FILE__, __LINE__, "Accepted client connection. socket #%d from %s:%d", client_socket, inet_ntoa(client->addr->sin_addr), ntohs(client->addr->sin_port));
    log_info(__FILE__, __LINE__, "Current connected clients : %d", server->cur_cons);
}


void server_event_read(struct bufferevent *bev, void *arg)
{
    client_t *client = (client_t*) arg;

    char data[4096];
    int res;
    char *line;

    memset(data, 0, 4096);


    res = evbuffer_add_buffer(client->buf, bufferevent_get_input(bev));

    while((line = evbuffer_readln(client->buf, NULL, EVBUFFER_EOL_CRLF)) != NULL)
    {
        log_debug(__FILE__, __LINE__, "Socket #%d - message - (%d) %s", client->socket, strlen(line), line);
        res = message_process(client, line);

        if(res == REQ_PROC_DISCONNECT)
        {
            log_debug(__FILE__, __LINE__, "Closing client connection. socket #%d", client->socket);
            client->server->cur_cons--;
            client_free(client);
            return;
        }
    }
    
    return;
}


void server_event_write(struct bufferevent *bev, void *arg)
{
    log_debug(__FILE__, __LINE__, "Event write.");
    return;
}

void server_event_error(struct bufferevent *bev, short events, void *arg)
{
    log_debug(__FILE__, __LINE__, "Received event error(%d)", events);

    if (events & BEV_EVENT_READING)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_READING");
    } else if (events & BEV_EVENT_WRITING)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_WRITING");
    } else if (events & BEV_EVENT_EOF)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_EOF");
    } else if (events & BEV_EVENT_ERROR)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_ERROR");
    } else if (events & BEV_EVENT_TIMEOUT)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_TIMEOUT");
    } else if (events & BEV_EVENT_CONNECTED)
    {
        log_debug(__FILE__, __LINE__, "Event error type : BEV_EVENT_CONNECTED");
    }

    client_t *client = (client_t*) arg;
    log_debug(__FILE__, __LINE__, "Client closed. #%d", client->socket);
    client->server->cur_cons--;
    client_free(client);
    return;
}

void server_event_error2(struct evconnlistener *listener, void *arg)
{
    log_debug(__FILE__, __LINE__, "Error event received. ERROR : %s",  evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
}

