#include "ssl.h"

#ifndef __CONN_H_
#define __CONN_H_

#define MAXCONN 20
#define MINCONN 2
#define THOLD 1
#define TIMEOUT 20

typedef struct {
	connection *connPool[MAXCONN];
	int connStatus[MAXCONN];
	int connTime[MAXCONN];
	int maxConn;
	int curConn;
	int increasePool;
	int decreasePool;

	/* Connection Details. */
	char host[1024], *url;
	int port;

	int getConnLock, freeConnLock;
} *conn, hConnPool;

int check(conn, char *, int);
int exists (conn *, int, char *, int);
conn newConn(char *, int);
int init(conn);
void addConn(conn);
int deleteConn(conn);
int getConn(conn);
int freeConn(conn, int);
void refresher(conn[], int);

#endif
