#include "conn.h"

int check(conn c, char *host, int port) {
	if(0 == strcmp(host, c.host) && port == c.port) return 1;
	return 0;
}

int exists (conn * hconn, int maxConn, char *host, int port) {
	int cur = 0;
	if(maxConn > 0) {
		while(cur < maxConn) {
			if(check(hconn[cur], host, port)) 
			   return cur;
			cur++;	
		}
	} else return -1;

	return -1;
}

conn newConn(char *host, int port) {
	conn c; 
	strcpy(c.host, host);
	c.port = port;
	init(c);

	return c;
}

int init(conn c) {
	int i;

	for(i=0; i<MINCONN; i++) {
		c.connPool[i] = sslConnect(c.host, c.port);
		c.connStatus[i] = 0;
		c.connTime[i] = time(NULL);
	}

	c.maxConn = MINCONN;
	c.curConn = 0;
	c.increasePool = 0;
	c.decreasePool = 0;

	return 1;
}

void addConn(conn c) {
	int curMax = c.maxConn;
	c.connPool[curMax] = sslConnect(c.host, c.port);
	c.connStatus[curMax] = 0;
	c.connTime[curMax] = time(NULL);

	c.maxConn++;
}

void deleteConn(conn c) {
	c.maxConn--;
	/* Free or not to free, that is the question. */
}

int getConn(conn c) {
	int track = 0, cur = c.curConn;
	
	while(c.connStatus[cur] > 0) {
		cur = (cur >= c.maxConn - 1) ? 0 : cur + 1;
		track++;
	}

	c.connStatus[cur] = 1; c.connTime[cur] = time(NULL);

	if(track > c.maxConn * THOLD) c.increasePool++;

	c.curConn = (cur >= c.maxConn - 1) ? 0 : cur + 1;

	return cur;
}

int freeConn(conn c, int p) {
	int wastedTime;

	wastedTime = c.connTime[p] - time(NULL);
	c.connTime[p] = time(NULL);
	c.connStatus[p] = 0;

	return wastedTime;
}
