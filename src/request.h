#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "meerkat.h"

request_t* request_init(int flag, char *line);
void request_free(request_t *request);

#endif // _REQUEST_H_
