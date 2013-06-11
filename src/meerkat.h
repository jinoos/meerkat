#ifndef _MEERKAT_H_
#define _MEERKAT_H_

#include <stdint.h>
#include <stdlib.h>
#include <event2/event.h>
#include <event2/buffer.h>

#define MEERKAT_NEWLINE "\r\n"

typedef enum _request_process_result
{
    REQ_PROC_DONE,          
    REQ_PROC_CONTINUE,
    REQ_PROC_INVALID,           // reset
    REQ_PROC_DISCONNECT         // disconnect
} request_process_result;

typedef enum _request_packet_status
{
    REQ_PKT_NONE,
    REQ_PKT_BFR_SERVICE,
    REQ_PKT_BFR_KEY
} request_packet_status;


typedef struct _service service_t;

typedef struct _server
{
    struct sockaddr_in      *addr;

    evutil_socket_t         socket;

    // relate with libevent
    struct event_base       *evbase;
    struct evconnlistener   *evlistener;
    struct event_base       *evsignal;

    // number of current connection
    int                     all_cons;

    // number of current connection
    int                     cur_cons;

    // max number of connection, limitation.
    int                     max_cons;

    // number of requests from clients
    uint64_t                num_reqs;

    // number of errors
    uint64_t                num_errs;

    service_t               **service_idx;

    int                     num_services;
    service_t               *service_first;
    service_t               *service_last;
} server_t;

typedef struct _request request_t;

typedef struct _client
{
    int                     socket;
    struct sockaddr_in      *addr;
    struct bufferevent      *bufev;
    struct evbuffer         *buf;
    server_t                *server;
    request_t               *request;
    request_packet_status   status;
} client_t;

typedef struct _request
{
    int     flag;

    char    *cmd;
    char    *service;
    char    *key;
} request_t;

typedef struct _service
{
    char        *name;
    size_t      len;
    service_t   *next;
    service_t   *all_next;
} service_t;

// for request
#define REQ_CMD_PING            "*PING"                 // no required any more
#define REQ_CMD_GETNODE         "*GET NODE"             // required service and key
#define REQ_CMD_ALLNODES        "*GET ALL NODES"        // required service
#define REQ_CMD_ALIVENODES      "*GET ALIVE NODES"      // required service
#define REQ_CMD_SERVICES        "*GET SERVICES"         // no required any more
#define REQ_CMD_STATUS          "*GET STATUS"           // no required any more
#define REQ_CMD_QUIT            "*QUIT"                 // no required any more

#define REQ_CMD_INT_PING        0x0001
#define REQ_CMD_INT_GETNODE     0x0002
#define REQ_CMD_INT_ALLNODES    0x0003
#define REQ_CMD_INT_ALIVENODES  0x0004
#define REQ_CMD_INT_SERVICES    0x0005
#define REQ_CMD_INT_STATUS      0x0006
#define REQ_CMD_INT_QUIT        0x0099


// for response
#define RESP_CMD_PONG           "+PONG"
#define RESP_CMD_NODE           "+NODE"
#define RESP_CMD_NODES          "+NODES"
#define RESP_CMD_SERVICES       "+SERVICES"
#define RESP_CMD_STATUS         "+STATUS"
#define RESP_CMD_STRING         "+STRING"
#define RESP_CMD_ERROR          "+ERROR"

#define RESP_ERR_WRONG_CMD                  0x0001
#define RESP_ERR_WRONG_CMD_STR              "Wrong command."
#define RESP_ERR_UNKNOWN_CMD                0x0002
#define RESP_ERR_UNKNOWN_CMD_STR            "Unknown command."
#define RESP_ERR_WRONG_PROTOCOL             0x0003
#define RESP_ERR_WRONG_PROTOCOL_STR         "Wrong protocol."

#endif // _MEERKAT_H_
