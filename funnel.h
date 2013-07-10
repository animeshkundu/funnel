#include <pthread.h>

#ifndef __HCONN_H_

#include "hconn.h"
#include "sig.h"

void * refreshConn(void *);
void * processRequest(void *);

#endif

