#include "ssl.h"
#include "jsmn.h"

#ifndef __conn_h
#define __conn_h

jsmn_parser jsmnP;
jsmntok_t jsmnTok[1000];
int jsmnR, jsmnCount = 0;

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

#endif
