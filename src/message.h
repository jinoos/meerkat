#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <event2/event.h>
#include <event2/buffer.h>

#include "meerkat.h"

int message_process(client_t *client, char *line);
void reset_request(client_t *client);
int send_error(client_t *client, int error_no, const char *error_msg);
int send_pong(client_t *client);
int send_error(client_t *client, int error_no, const char *error_msg);
int send_get_node(client_t *client);
int send_get_all_nodes(client_t *client);
int send_get_alive_nodes(client_t *client);
int send_get_services(client_t *client);
int send_get_status(client_t *client);

// int parse_buffer(client_t *client, struct evbuffer *buf);

#endif // _MESSAGE_H_
