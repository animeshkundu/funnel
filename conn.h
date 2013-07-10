#include "ssl.h"

#ifndef __CONN_H_
#define __CONN_H_ 1

#define MAXCONN 	50
#define MINCONN 	2
#define THOLD 		1
#define TIMEOUT 	1800

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

int check(conn, char *, int);
int exists (conn *, int, char *, int);
conn newSslConn(char *, int);
conn newTcpConn(char *, int);
int initSsl(conn);
void addConn(conn);
int deleteConn(conn);
int getConn(conn);
int freeConn(conn, int);
void refresher(conn[], int);

#endif
