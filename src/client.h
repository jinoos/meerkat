#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <event2/event.h>
#include <event2/buffer.h>

#include "meerkat.h"

void client_free(client_t *client);

#endif // _CLIENT_H_
