#include "ssl.h"

#ifndef __conn_h_
#define __conn_h_

#define MAXCONN 20
#define MINCONN 1
#define THOLD 1
#define TIMEOUT 1800

typedef struct {
	connection *connPool[MAXCONN];
	int connStatus[MAXCONN];
	int connTime[MAXCONN];
	int maxConn;
	int curConn;
	int increasePool;
	int decreasePool;

	/* Connection Details. */
	char *host, *url;
	int port;
} conn;

conn newConn(char *, int);

#endif
