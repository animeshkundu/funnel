#include "hconn.h"

static __thread int jsmnCount = 0;
static __thread int freeSock = -1;
static __thread int freeCsock = -1;
static __thread conn freeStruct = NULL;

int reconnect(conn c, int sock) {
	
	if(443 == c->port) {
		sslDisconnect(c->connPool[sock]);

		/* Should have error handling. */
		connection *tmp = sslConnect(c->host, c->port);
		
		c->connPool[sock] = tmp;
		LOGD(5, "Reconnected Disconnected connection : ", tmp->socket);
	} else {
		close(c->tcpConnPool[sock]);
		int tmp = tcpConnect(c->host, c->port);
		if(tmp < 0) return tmp;
		c->tcpConnPool[sock] = tmp;
		LOGD(5, "Reconnected Disconnected connection : ", (int) tmp);
	}

	return 1;
}

int handleSSL(conn hconn[], int cur, char data[], char response[]) {
	int sockNo;
	connection *sock;
	
	sockNo = getConn(hconn[cur]);
	LOGD(0, "Socket No : ", sockNo);
	sock = hconn[cur]->connPool[sockNo];
	
	if(sock < 0) {
		reconnect(hconn[cur], sockNo);
		sock = hconn[cur]->connPool[sockNo];
	}

	LOGD(0, "Got Socket : ", sock->socket);
	LOGV(5, "Request sent to Bank.", data);
	
	if (sslWrite(sock, data) <= 0) {
		reconnect(hconn[cur], sockNo);
		sock = hconn[cur]->connPool[sockNo];
		LOGD(5, "Socket : ", sock->socket);
		if(sslWrite(sock, data) <= 0) { 
			freeConn(hconn[cur], sockNo);
			freeStruct = NULL; freeCsock = -1;
			return -1; 
		}
	}

	if(sslRead(sock, response) <= 0) { 
		freeConn(hconn[cur], sockNo); 	
		freeStruct = NULL; freeCsock = -1;
		return -1; 
	}
	
	return sockNo;
}

int handleTCP(conn hconn[], int cur, char data[], char response[]) {
	int sockNo, sock;
	
	sockNo = getConn(hconn[cur]);
	LOGD(0, "Socket No : ", sockNo);
	sock = hconn[cur]->tcpConnPool[sockNo];
	
	if(sock < 0) {
		if(reconnect(hconn[cur], sockNo) < 0) { 
			freeConn(hconn[cur], sockNo); 	
			freeStruct = NULL; freeCsock = -1;
			return -1; 
		}
		sock = hconn[cur]->tcpConnPool[sockNo];
	}

	LOGD(0, "Got Socket : ", sock);
	LOGV(5, "Request sent to Bank.", data);
	
	/* Reconnect if neccessary. */
	if (tcpWrite(sock, data) < 0) {
		if(reconnect(hconn[cur], sockNo) < 0) { 
			freeConn(hconn[cur], sockNo); 	
			freeStruct = NULL; freeCsock = -1;
			return -1; 
		}
		sock = hconn[cur]->tcpConnPool[sockNo];
		LOGD(6, "Socket : ", sock);
		if(tcpWrite(sock, data) < 0) { 
			freeConn(hconn[cur], sockNo); 	
			freeStruct = NULL; freeCsock = -1;
			return -1; 
		}
	}

	if(tcpRead(sock, response) < 0) { 
		freeConn(hconn[cur], sockNo); 
		freeStruct = NULL; freeCsock = -1;
		return -1; 
	}

	return sockNo;
}

int handleRequest(int cSock, conn hconn[], int maxConn) {
	int cur, port;
	char request[40960], response[40960], host[1024], data[40960], tmp[40960];

	freeSock = cSock;
	LOG(0, "Inside handleRequest.");

	/* Handle signals per thread. */
	registerSignalHandler();

	if(tcpRead(cSock, request) < 0) { close(cSock); freeSock = -1; return -1; }
	if(strlen(request) == 0) { close(cSock); freeSock = -1; return -1; }
	if(processRead(request) < 0) { close(cSock); freeSock = -1; return -1; }
	LOGV(5, "Request : ", request);

	while(getToken(request, tmp)) {
		if(!strcmp("host", tmp)) getToken(request, host);
		if(!strcmp("port", tmp)) { getToken(request, tmp); port = atoi(tmp); }
		if(!strcmp("data", tmp)) getToken(request, data);
	} 
	
	LOGV(5, "Host : ", host);
	LOGD(5, "Port : ", port);
	LOGV(6, "Data : ", data);

	cur = exists(hconn, maxConn, host, port);
	freeStruct = hconn[cur];
	freeCsock = cur;

	LOGD(0, "Connection Exists : ", cur);

	if(cur < 0) {
		if(443 == port) {
			hconn[maxConn] = newSslConn(host, port);
			LOGD(0, "Created new hconn", hconn[maxConn]->connPool[0]->socket);
		} else {
			hconn[maxConn] = newTcpConn(host, port);
			LOGD(0, "Created new hconn", hconn[maxConn]->tcpConnPool[0]);
		}
		cur = maxConn; maxConn++;
	}
	
	int retVal = -1;
	/* Handle both types of connections. */
	if(443 == hconn[cur]->port) retVal = handleSSL(hconn, cur, data, response);
	else retVal = handleTCP(hconn, cur, data, response);

	if(retVal < 0) { close(cSock); freeSock = 1; return -1; }

	LOGV(6, "Response : ", response);
	strcat(response, "\r\n"); 
	
	if(tcpWrite(cSock, response) < 0) { 
		freeConn(hconn[cur], retVal); 	
		freeStruct = NULL; freeCsock = -1;
		close(cSock); freeSock = -1;
		return -1; 
	}
	
	LOG(0, "Sent response to client.");
	int timeTaken = freeConn(hconn[cur], retVal);
	LOGD(0, "Time taken : ", timeTaken);
	
	/* Allow the socket to drain. */
	usleep(50000); close(cSock);
	return maxConn;
}

int processRead(char req[]) {
	int i=0, j=0, len = strlen(req);
	while(req[i]!='{' && req[i+1]!='"') i++;
	if(i < len-i) strncpy(req, req + i, len - i);
	while(req[len-i+j]!=0) { req[len-i+j] = 0; j++; }
	return 1;
}

int getToken(const char * js, char data[]) {
	
	if(0 == jsmnCount) {
		jsmn_init(&jsmnP);
		jsmnR = jsmn_parse(&jsmnP, js, jsmnTok, 10);
		if(jsmnR != JSMN_SUCCESS) {
			LOGV(0, "Cannot Decode JSON", js);
			jsmnCount = 0;
			return 0;
		}
	} else if(jsmnTok[jsmnCount].start == 0) {
		jsmnCount = 0;
		return 0;
	}

	int i, k=0;
	for(i = jsmnTok[jsmnCount].start; i<jsmnTok[jsmnCount].end; i++) {
		if (js[i] == 92) {
			if (js[i+1] == 47 || js[i+1] == 92) data[k++] = js[i+1]; 
			else if (js[i+1] == 'r') data[k++] = '\r';
			else if (js[i+1] == 'n') data[k++] = '\n';
			else data[k++] = js[i];
			i++; continue;
		}
		data[k++] = js[i];
	}

	data[k] = 0; jsmnCount++; return 1;
}
