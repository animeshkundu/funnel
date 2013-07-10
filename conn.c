#include "conn.h"

int check(conn c, char host[], int port) {
	LOGV(0, "Checking ", c->host); LOG(0, "");
	if(0 == strcmp(host, c->host) && port == c->port) return 1;
	return 0;
}

int exists (conn hconn[], int maxConn, char host[], int port) {
	LOGD(0, "Max Connections : ", maxConn);
	if(NULL == hconn) return -1;

	int cur = 0;
	if(maxConn > 0) {
		while(cur < maxConn) {
			if(check(hconn[cur], host, port)) {
				LOGD(0, "Connection Found", cur);
			   	return cur;
			}
			cur++;	
		}
	} else return -1;

	/* Redundant. */
	return -1;
}

void refresher(conn hconn[], int maxConn) {
	if (maxConn <= 0) {
	   usleep(5000); return;
	}
	
	int cur = 0, inCur, curTime;
	while(cur < maxConn) {

		/* Loop through connection pool looking to delete connections. */
		inCur = 0; curTime = time(NULL);
		while(inCur < hconn[cur]->maxConn) {

			/* Alive connection check should be handled while reading or writing. NOT HERE. */

			if( (curTime - hconn[cur]->connTime[inCur]) > TIMEOUT )
				hconn[cur]->decreasePool++;
			
			inCur++;
		}

		if(hconn[cur]->decreasePool > 0 && 0 == hconn[cur]->increasePool) {
			if(deleteConn(hconn[cur]) > 0) { 
				hconn[cur]->decreasePool = 0;
				LOGD(6, "Deleted connection. ConnPool : ", cur);
			}
		} else if(hconn[cur]->increasePool > 0 && 0 == hconn[cur]->decreasePool) {
			addConn(hconn[cur]); hconn[cur]->increasePool = 0;
			LOGD(6, "Added connection. ConnPool : ", cur);
		} else if(hconn[cur]->increasePool > 0 && hconn[cur]->decreasePool > 0) {
			LOGD(6, "Neutralized : ", cur);
			hconn[cur]->increasePool = 0; hconn[cur]->decreasePool = 0;
		}

		cur++; usleep(500);
	}	
}

conn newTcpConn(char host[], int port) {
	int i; conn c;

	c = (conn) malloc (sizeof(hConnPool));
	strcpy(c->host, host);
	c->port = port;

	for(i=0; i<MINCONN; i++) {
		c->tcpConnPool[i] = tcpConnect(c->host, c->port);
		c->connStatus[i] = 0;
		c->connTime[i] = time(NULL);
		LOGD(0, "Created TCP Connection : ", c->tcpConnPool[i]);
	}

	c->maxConn = MINCONN;
	c->curConn = 0;
	c->increasePool = 0;
	c->decreasePool = 0;
	c->getConnLock = 0;

	return c;
}

conn newSslConn(char host[], int port) {
	conn c;
	
	c = (conn) malloc (sizeof(hConnPool));
	strcpy(c->host, host);
	c->port = port;
	
	initSsl(c);

	LOG(0, "Created new connection.");
	return c;
}

int initSsl(conn c) {
	int i;

	for(i=0; i<MINCONN; i++) {
		c->connPool[i] = sslConnect(c->host, c->port);
		c->connStatus[i] = 0;
		c->connTime[i] = time(NULL);
		LOGD(0, "Created SSL Connection : ", c->connPool[i]->socket);
	}

	c->maxConn = MINCONN;
	c->curConn = 0;
	c->increasePool = 0;
	c->decreasePool = 0;
	c->getConnLock = 0;

	return 1;
}

void addConn(conn c) {
	/* Limit upper boundary. */
	if(c->maxConn >= MAXCONN) return;

	int curMax = c->maxConn;

	if(443 == c->port) c->connPool[curMax] = sslConnect(c->host, c->port);
	else c->tcpConnPool[curMax] = tcpConnect(c->host, c->port);

	c->connStatus[curMax] = 0;
	c->connTime[curMax] = time(NULL);

	c->maxConn++;
	LOGD(6, "Added Connection", c->maxConn);
}

int deleteConn(conn c) {
	/* Limit lower boundary. */
	if(c->maxConn > MINCONN && ((443 == c->port && 0 == c->connStatus[c->maxConn]) || 0 == c->tcpConnPool[c->maxConn])) {
		c->maxConn--;
		LOGV(6, "Deleted Connection", c->host);
		/* Free or not to free, that is the question. */
		return 1;
	} else return -1;
}

int getConn(conn c) {
	int track = 0, cur = c->curConn;
	
	/* Function level locks, not a good idea without sufficient proof. */
	while(c->getConnLock > 0) usleep(500);
	c->getConnLock = 1;

	while(c->connStatus[cur] > 0) {
		cur = (cur >= c->maxConn - 1) ? 0 : cur + 1;
		track++;

		/* Explicit fallback. If this is happening frequently, there is a problem with refresher function. */
		if(track > (c->maxConn * 3 * THOLD)) {
			addConn(c); track = 0;
			LOGD(10, "Notice : Forced Increase of connection pool. Max : ", c->maxConn);
		}
	}

	c->connStatus[cur] = 1; c->connTime[cur] = time(NULL);
	c->getConnLock = 0;

	if(track > c->maxConn * THOLD) c->increasePool++;
	LOGD(0, "Current Track : ", track);

	c->curConn = (cur >= c->maxConn - 1) ? 0 : cur + 1;

	if(c->connPool[cur] < 0) {
		if(443 == c->port) {
			sslDisconnect(c->connPool[cur]);
			c->connPool[cur] = sslConnect(c->host, c->port);
		} else {
			close(c->tcpConnPool[cur]);
			c->tcpConnPool[cur] = tcpConnect(c->host, c->port);
		}
		LOGD(6, "Reconnected disconnected connection : ", cur);
	}

	return cur;
}

int freeConn(conn c, int p) {
	int wastedTime;

	wastedTime = c->connTime[p] - time(NULL);
	c->connTime[p] = time(NULL);
	c->connStatus[p] = 0;

	return wastedTime;
}
