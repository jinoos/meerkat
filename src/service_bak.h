#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "meerkat.h"

service_t* service_init(char *name);
service_t** service_index_init(int power);
service_t* service_find2(service_t** idx, int power, char *svc_name, size_t svc_len);
service_t* service_find(service_t** idx, int power, char *svc_name);
service_t* service_add(service_t** idx, int power, service_t* svc);

#endif // _SERVICE_H_
