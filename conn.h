#include "ssl.h"

#ifndef __CONN_H_
#define __CONN_H_

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
} *conn, hConnPool;

int check(conn, char *, int);
int exists (conn *, int, char *, int);
conn newConn(char *, int);
int init(conn);
void addConn(conn);
void deleteConn(conn);
int getConn(conn);
int freeConn(conn, int);

#endif
