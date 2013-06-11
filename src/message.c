#include <string.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "logging.h"
#include "request.h"
#include "message.h"

int message_process(client_t *client, char *line)
{
    if(line == NULL)
    {
        return REQ_PROC_INVALID;
    }

    int line_size = strlen(line);

    if(line_size == 0)
    {
        log_info(__FILE__, __LINE__, "Empty command. Reset request.");
        request_free(client->request);
        free(line);
        client->status = REQ_PKT_NONE;
        client->server->num_errs++;
        return REQ_PROC_INVALID;
    }

    if(client->status == REQ_PKT_NONE)
    {
        // Check invalid command
        if(line[0] != 0x2A) 
        {
            log_info(__FILE__, __LINE__, "Wrong command recieved. Received message : %s", line);
            free(line);
            send_error(client, RESP_ERR_WRONG_CMD, RESP_ERR_WRONG_CMD_STR);
            client->server->num_errs++;
            return REQ_PROC_INVALID;
        }

        // 
        if(strcasecmp(line, REQ_CMD_GETNODE) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received GET NODE command.");
            client->request = request_init(REQ_CMD_INT_GETNODE, line);
            client->status = REQ_PKT_BFR_SERVICE;
            return REQ_PROC_CONTINUE;
        }

        if(strcasecmp(line, REQ_CMD_PING) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received PING command.");
            send_pong(client);
            free(line);
            return REQ_PROC_DONE;
        }

        if(strcasecmp(line, REQ_CMD_QUIT) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received QUIT command.");
            free(line);
            return REQ_PROC_DISCONNECT;
        }

        if(strcasecmp(line, REQ_CMD_ALLNODES) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received ALL NODES command.");
            client->request = request_init(REQ_CMD_INT_ALLNODES, line);
            client->status = REQ_PKT_BFR_SERVICE;
            return REQ_PROC_CONTINUE;
        }

        if(strcasecmp(line, REQ_CMD_ALIVENODES) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received ALIVE NODES command.");
            client->request = request_init(REQ_CMD_INT_ALIVENODES, line);
            client->status = REQ_PKT_BFR_SERVICE;
            return REQ_PROC_CONTINUE;
        }

        if(strcasecmp(line, REQ_CMD_SERVICES) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received SERVICES command.");
            send_get_services(client);
            free(line);
            return REQ_PROC_DONE;
        }

        if(strcasecmp(line, REQ_CMD_STATUS) == 0)
        {
            log_debug(__FILE__, __LINE__, "Received STATUS command.");
            send_get_status(client);
            free(line);
            return REQ_PROC_DONE;
        }

        log_debug(__FILE__, __LINE__, "Unknown command recieved. Received message : %s", line);
        free(line);
        send_error(client, RESP_ERR_UNKNOWN_CMD, RESP_ERR_UNKNOWN_CMD_STR);
        client->server->num_errs++;
        return REQ_PROC_INVALID;

    // for SERVICE
    }else if(client->status == REQ_PKT_BFR_SERVICE)
    {
        int res;

        log_debug(__FILE__, __LINE__, "Received SERVICE.");

        switch(client->request->flag)
        {
            // for GET SER
            case REQ_CMD_INT_GETNODE:
                client->request->service = line;
                client->status = REQ_PKT_BFR_KEY;
                return REQ_PROC_CONTINUE;
                break;

            case REQ_CMD_INT_ALLNODES:
                client->request->service = line;
                res = send_get_all_nodes(client);
                client->status = REQ_PKT_NONE;
                request_free(client->request);
                return res;
                break;

            case REQ_CMD_INT_ALIVENODES:
                client->request->service = line;
                res = send_get_alive_nodes(client);
                client->status = REQ_PKT_NONE;
                request_free(client->request);
                return res;
                break;

            default:
                request_free(client->request);
                free(line);
                send_error(client, RESP_ERR_WRONG_PROTOCOL, RESP_ERR_WRONG_PROTOCOL_STR);
                client->status = REQ_PKT_NONE;
                client->server->num_errs++;
                return REQ_PROC_INVALID;
                break;
        }

    // FOR KEY
    }else if(client->status == REQ_PKT_BFR_KEY)
    {
        int res;

        log_debug(__FILE__, __LINE__, "Received KEY.");

        // for GET NODE
        switch(client->request->flag)
        {
            case REQ_CMD_INT_GETNODE:
                client->request->key = line;
                res = send_get_node(client);
                client->status = REQ_PKT_NONE;
                request_free(client->request);
                return res;
                break;

            default:
                request_free(client->request);
                free(line);
                send_error(client, RESP_ERR_WRONG_PROTOCOL, RESP_ERR_WRONG_PROTOCOL_STR);
                client->server->num_errs++;
                return REQ_PROC_INVALID;
                break;
        }
    }

    request_free(client->request);
    free(line);
    send_error(client, RESP_ERR_WRONG_PROTOCOL, RESP_ERR_WRONG_PROTOCOL_STR);
    client->server->num_errs++;
    return REQ_PROC_INVALID;
}

int send_pong(client_t *client)
{
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s", RESP_CMD_PONG, MEERKAT_NEWLINE);
    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_error(client_t *client, int error_no, const char *error_msg)
{
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s%d%s%s%s", RESP_CMD_ERROR, MEERKAT_NEWLINE, error_no, MEERKAT_NEWLINE, error_msg, MEERKAT_NEWLINE);
    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_get_node(client_t *client)
{
    //
    // requred detail code
    //
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s%s%s", RESP_CMD_NODE, MEERKAT_NEWLINE, "hostname:1234", MEERKAT_NEWLINE);
    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_get_all_nodes(client_t *client)
{
    //
    // requred detail code
    //
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s%s%s%s%s", RESP_CMD_NODES, MEERKAT_NEWLINE, "3", MEERKAT_NEWLINE, "hostname1:1234"MEERKAT_NEWLINE"hostname2:1235"MEERKAT_NEWLINE"hostname3:12346", MEERKAT_NEWLINE);
    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_get_alive_nodes(client_t *client)
{
    //
    // requred detail code
    //
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s%s%s%s%s", RESP_CMD_NODES, MEERKAT_NEWLINE, "2", MEERKAT_NEWLINE, "hostname1:1234"MEERKAT_NEWLINE"hostname2:1235", MEERKAT_NEWLINE);
    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_get_services(client_t *client)
{
    //
    // requred detail code
    //
    int res;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s%d%s", RESP_CMD_SERVICES, MEERKAT_NEWLINE, client->server->num_services, MEERKAT_NEWLINE);

    service_t *service = client->server->service_first;

    while(service != NULL)
    {
        evbuffer_add_printf(buf, "%s%s", service->name, MEERKAT_NEWLINE);
        service = service->all_next;
    }

    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}

int send_get_status(client_t *client)
{
    //
    // requred detail code
    //
    int res;
    server_t *server = client->server;
    struct evbuffer *buf = evbuffer_new();
    evbuffer_add_printf(buf, "%s%s", RESP_CMD_STATUS, MEERKAT_NEWLINE);
    evbuffer_add_printf(buf, "{");

    evbuffer_add_printf(buf, "\"all_connections\": %d, ",           server->all_cons);
    evbuffer_add_printf(buf, "\"max_connections\": %d, ",           server->max_cons);
    evbuffer_add_printf(buf, "\"current_connections\": %d, ",       server->cur_cons);
    evbuffer_add_printf(buf, "\"number_of_requests\": %lu, ",        server->num_reqs);
    evbuffer_add_printf(buf, "\"number_of_errors\": %lu ",           server->num_errs);

    evbuffer_add_printf(buf, "}%s", MEERKAT_NEWLINE);

    res = bufferevent_write_buffer(client->bufev, buf);
    evbuffer_free(buf);
    client->server->num_reqs++;
    return res;
}


