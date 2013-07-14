#ifndef __CONN_H_
#define __CONN_H_ 

#include "ssl.h"

typedef struct {
	connection *connPool[MAXCONN];
	int tcpConnPool[MAXCONN];
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

int exists (conn *, int, char *, int);
conn newSslConn(char *, int);
conn newTcpConn(char *, int);
int getConn(conn);
int freeConn(conn, int);
void refresher(conn[], int);

#endif
