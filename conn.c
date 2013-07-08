#include "conn.h"

int check(conn c, char *host, int port) {
	LOGV(0, "Checking ", c->host); LOG(0, "");
	if(0 == strcmp(host, c->host) && port == c->port) return 1;
	return 0;
}

int exists (conn *hconn, int maxConn, char *host, int port) {
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

	return -1;
}

void refresher(conn *hconn, int maxConn) {
	if (maxConn <= 0) {
	   usleep(5000);
   	   return;
	}
	
	int cur = 0, inCur, curTime;
	while(cur < maxConn) {
		/* Loop through connection pool looking to delete connections. */
		inCur = 0; curTime = time(NULL);
		while(inCur < hconn[cur]->maxConn) {
			/*if(strlen(sslRead(hconn[cur]->connPool[inCur])) < 0 && hconn[cur]->connStatus[inCur] == 0) {
				hconn[cur]->connStatus[inCur] = 1;
				sslDisconnect(hconn[cur]->connPool[inCur]);
				hconn[cur]->connPool[inCur] = sslConnect(hconn[cur]->host, hconn[cur]->port);
				LOGD(5, "Reconnected disconnected connection", inCur);
				hconn[cur]->connStatus[inCur] = 0;
			}*/

			if( (curTime - hconn[cur]->connTime[inCur]) > TIMEOUT )
				hconn[cur]->decreasePool++;
			
			inCur++;
		}

		if(hconn[cur]->decreasePool > 0 && 0 == hconn[cur]->increasePool) {
			deleteConn(hconn[cur]); hconn[cur]->decreasePool = 0;
			LOGD(5, "Deleted connection. ConnPool : ", cur);
		} else if(hconn[cur]->increasePool > 0 && 0 == hconn[cur]->decreasePool) {
			addConn(hconn[cur]); hconn[cur]->increasePool = 0;
			LOGD(5, "Added connection. ConnPool : ", cur);
		} else if(hconn[cur]->increasePool > 0 && hconn[cur]->decreasePool > 0) {
			LOGD(5, "Neutralized : ", cur);
			hconn[cur]->increasePool = 0; hconn[cur]->decreasePool = 0;
		}

		cur++; usleep(500);
	}	
}

conn newConn(char *host, int port) {
	conn c;	
	c = (conn) malloc (sizeof(hConnPool));
	c->host = (char *) malloc ((strlen(host) + 1) * sizeof(char));
	strcpy(c->host, host);
	c->port = port;
	init(c);
	LOG(0, "Created new connection.");
	return c;
}

int init(conn c) {
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

	return 1;
}

void addConn(conn c) {
	int curMax = c->maxConn;
	c->connPool[curMax] = sslConnect(c->host, c->port);
	c->connStatus[curMax] = 0;
	c->connTime[curMax] = time(NULL);

	c->maxConn++;
	LOGV(5, "Added Connection", c->host);
}

void deleteConn(conn c) {
	c->maxConn--;
	LOGV(5, "Deleted Connection", c->host);
	/* Free or not to free, that is the question. */
}

int getConn(conn c) {
	int track = 0, cur = c->curConn;
	
	while(c->connStatus[cur] > 0) {
		cur = (cur >= c->maxConn - 1) ? 0 : cur + 1;
		track++;
		if(track > c->maxConn * THOLD) {
			addConn(c); track = 0;
		}
		usleep(50);
	}

	c->connStatus[cur] = 1; c->connTime[cur] = time(NULL);

	if(track > c->maxConn * THOLD) c->increasePool++;
	LOGD(0, "Current Track : ", track);

	c->curConn = (cur >= c->maxConn - 1) ? 0 : cur + 1;

	if(c->connPool[cur] < 0) {
		sslDisconnect(c->connPool[cur]);
		c->connPool[cur] = sslConnect(c->host, c->port);
		LOGD(0, "Reconnected disconnected connection : ", cur);
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


