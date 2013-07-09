#include "hconn.h"

int jsmnCount = 0;

int reconnect(conn c, int sock) {
	sslDisconnect(c->connPool[sock]);
	connection *tmp = sslConnect(c->host, c->port);
	c->connPool[sock] = tmp;
	LOGD(6, "Reconnected Disconnected connection : ", tmp->socket);
	return 1;
}

int handleRequest(int cSock, conn hconn[], int maxConn) {
	int cur, sockNo, port;
	connection *sock;
	char request[40960], response[40960], host[1024], data[40960], tmp[40960];

	LOG(0, "Inside handleRequest.");
	if(tcpRead(cSock, request) < 0) { close(cSock); return -1; }

	processRead(request);
	LOGV(0, "Request : ", request);
	while(getToken(request, tmp)) {
		/*if(tmp == NULL) {
			LOGV(10, "JSON String not properly formatted.", request);
			free(tmp); free(request);
			return maxConn;
		}*/
		if(!strcmp("host", tmp)) getToken(request, host);
		if(!strcmp("port", tmp)) { getToken(request, tmp); port = atoi(tmp); }
		if(!strcmp("data", tmp)) getToken(request, data);
	} 
	
	//free(tmp); free(request);
	
	LOGV(6, "Host : ", host);
	LOGD(6, "Port : ", port);
	LOGV(0, "Data : ", data); LOG(0, "");
	cur = exists(hconn, maxConn, host, port);
	LOGD(0, "Connection Exists : ", cur);

	if(cur < 0) {
		//hconn = (NULL == hconn) ? ((conn *) malloc (sizeof(hConnPool))) : ((conn *) realloc (hconn, (sizeof(hConnPool) * (maxConn + 1))));
		//LOG(0, "Malloced or realloced hconn");
		hconn[maxConn] = newConn(host, port);
		LOGD(0, "Created new hconn", hconn[maxConn]->connPool[0]->socket);
		cur = maxConn; maxConn++;
	}
	
	sockNo = getConn(hconn[cur]);
	LOGD(0, "Socket No : ", sockNo);
	sock = hconn[cur]->connPool[sockNo];
	
	if(sock < 0) {
		/*sslDisconnect(hconn[cur]->connPool[sockNo]);
		hconn[cur]->connPool[sockNo] = sslConnect(hconn[cur]->host, hconn[cur]->port);*/
		reconnect(hconn[cur], sockNo);
		sock = hconn[cur]->connPool[sockNo];
	}

	LOGD(0, "Got Socket : ", sock->socket);
	LOGV(5, "Request sent to Bank.", data);
	
	if (sslWrite(sock, data) < 0) {
		reconnect(hconn[cur], sockNo);
		sock = hconn[cur]->connPool[sockNo];
		LOGD(6, "Socket : ", sock->socket);
		if(sslWrite(sock, data) < 0) { freeConn(hconn[cur], cur); close(cSock); return -1; }
	}

	if(sslRead(sock, response) < 0) { freeConn(hconn[cur], cur); close(cSock); return -1; }

	LOGV(5, "Response : ", response);
	strcat(response, "\r\n");
	tcpWrite(cSock, response);
	LOG(0, "Sent response to client.");

	//sslDisconnect(hconn[cur]->connPool[sockNo]);
	//hconn[cur]->connPool[sockNo] = sslConnect(hconn[cur]->host, hconn[cur]->port);

	int timeTaken = freeConn(hconn[cur], cur);
	LOGD(0, "Time taken : ", timeTaken);
	
	//shutdown(cSock, SHUT_WR);	
	//while(strlen(tcpRead(cSock)) > 0) usleep(500);
	usleep(500); close(cSock);

	//free(host); free(response);
	return maxConn;
}

int processRead(char req[]) {
	int i=0, j=0, len = strlen(req);
	while(req[i]!='{' && req[i+1]!='"') i++;
	strncpy(req, req + i, len - i);
	while(req[len-i+j]!=0) {
		req[len-i+j] = 0; j++;
	}
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

	//char * data;
	int i, k=0;
	//data = (char *) malloc ( sizeof(char) * (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start + 1) );
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
	data[k] = 0;	
	/*strncpy (data, js+jsmnTok[jsmnCount].start, (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start));*/
	jsmnCount++;

	return 1;
}
