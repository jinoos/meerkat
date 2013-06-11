#ifndef _SERVER_EVENT_H_
#define _SERVER_EVENT_H_

#include <event2/event.h>
#include "server.h"
#include "client.h"

#define SERVER_DEFAULT_PORT 7758

server_t* server_init(int port);
void server_free(server_t *server);

struct event_base* server_event_base_init();
void server_event_run(server_t *server);
void server_event_accept(struct evconnlistener *listener, evutil_socket_t client_socket, struct sockaddr *sa, int socklen, void *arg);
void server_event_read(struct bufferevent *bev, void *arg);
void server_event_write(struct bufferevent *bev, void *arg);
void server_event_error(struct bufferevent *bev, short events, void *arg);
void server_event_error2(struct evconnlistener *listener, void *arg);

#endif // _SERVER_EVENT_H_
