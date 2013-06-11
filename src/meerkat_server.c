#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

#include "meerkat.h"
#include "server.h"
#include "service.h"
#include "logging.h"

#define DEFAULT_PORT    7758

static void write_to_file_cb(int severity, const char *msg)
{
    log_debug(__FILE__, __LINE__, "libevent - %s", msg);
}

int main(const int argc, const char *argv[])
{
    event_set_log_callback(write_to_file_cb);
    event_enable_debug_mode();

    //log_level(LOG_DEBUG);
    log_fileline(LOG_FILELINE_ON);
    log_level(LOG_WARN);

    server_t *server = server_init(DEFAULT_PORT);

    if(server == NULL)
    {
        log_err(__FILE__, __LINE__, "Cannot init server.");
        exit(-1);
    }

    int power = 10;
    server->service_idx = service_index_init(power);
    char *str = NULL;

    str = calloc(sizeof(char), 5);
    memcpy(str, "aaaa", 4);
    service_t *aaaa = service_init(str);
    aaaa = service_add(server->service_idx, power, aaaa);

    server->service_first = aaaa;
    server->service_last  = aaaa;
    server->num_services++;

    str = calloc(sizeof(char), 5);
    memcpy(str, "bbbb", 4);
    service_t *bbbb = service_init(str);
    bbbb = service_add(server->service_idx, power, bbbb);

    aaaa->all_next = bbbb;
    server->service_last = bbbb;
    server->num_services++;

    str = calloc(sizeof(char), 5);
    memcpy(str, "cccc", 4);
    service_t *cccc = service_init(str);
    cccc = service_add(server->service_idx, power, cccc);

    bbbb->all_next = cccc;
    server->service_last = cccc;
    server->num_services++;

    server_event_run(server);

    return 0;
}
