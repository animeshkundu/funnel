#include "hconn.h"

jsmnCount = 0;

char *getToken(const char * js) {
	if(0 == jsmnCount) {
		jsmn_init(&jsmnP);
		jsmnR = jsmn_parse(&jsmnP, js, jsmnTok, 10);
		if(jsmnR != JSMN_SUCCESS) {
			LOGV(0, "Cannot Decode JSON", js);
			jsmnCount = 0;
			return NULL;
		}
	} else if(jsmnTok[jsmnCount].start == 0) {
		jsmnCount = 0;
		return NULL;
	}

	char * data; int i, k=0;
	data = (char *) malloc ( sizeof(char) * (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start + 1) );
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
	data[k] = '\0';	
	/*strncpy (data, js+jsmnTok[jsmnCount].start, (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start));*/
	jsmnCount++;

	return data;
}

char processRead(char * req) {
	int i=0, j=0, len = strlen(req);
	while(req[i]!='{' && req[i+1]!='"') i++;
	strncpy(req, req + i, len - i);
	while(req[len-i+j]!=0) {
		req[len-i+j] = 0; j++;
	}
}

int handleRequest(int cSock, conn *hconn, int maxConn) {
	int cur, sockNo, port;
	connection *sock;
	char *request, *response, *host, *data, *tmp;

	LOG(0, "Inside handleRequest.");
	request = tcpRead(cSock);
	processRead(request);
	LOGV(0, "Request : ", request);
	while(tmp = getToken(request)) {
		if(tmp == NULL) {
			LOGV(10, "JSON String not properly formatted.", request);
			free(tmp); free(request);
			return maxConn;
		}
		if(!strcmp("host", tmp)) host = getToken(request);
		if(!strcmp("port", tmp)) port = atoi(getToken(request));
		if(!strcmp("data", tmp)) data = getToken(request);
	} 
	
	free(tmp); free(request);
	
	LOGV(0, "Host : ", host);
	LOGD(0, "Port : ", port);
	LOGV(0, "Data : ", data); LOG(0, "");
	cur = exists(hconn, maxConn, host, port);
	LOGD(0, "Connection Exists : ", cur);

	if(cur < 0) {
		hconn = (NULL == hconn) ? ((conn *) malloc (sizeof(hConnPool))) : (realloc (hconn, (sizeof(hConnPool) * (maxConn + 1))));
		LOG(0, "Malloced or realloced hconn");
		hconn[maxConn] = newConn(host, port);
		LOGD(0, "Created new hconn", hconn[maxConn]->connPool[0]->socket);
		cur = maxConn; maxConn++;
	}
	
	sockNo = getConn(hconn[cur]);
	LOGD(0, "Socket No : ", sockNo);
	sock = hconn[cur]->connPool[sockNo];
	
	if(sock < 0) {
		sslDisconnect(hconn[cur]->connPool[sockNo]);
		hconn[cur]->connPool[sockNo] = sslConnect(hconn[cur]->host, hconn[cur]->port);
		LOGD(0, "Reconnected Disconnected connection", cur);
		sock = hconn[cur]->connPool[sockNo];
	}

	LOGD(0, "Got Socket : ", sock->socket);
	sslWrite(sock, data);
	LOG(0, "Request sent to Bank.");
	response = sslRead(sock);
	LOGV(5, "Response : ", response);
	strcat(response, "\r\n");
	tcpWrite(cSock, response);
	LOG(0, "Sent response to client.");

	//sslDisconnect(hconn[cur]->connPool[sockNo]);
	//hconn[cur]->connPool[sockNo] = sslConnect(hconn[cur]->host, hconn[cur]->port);

	int timeTaken = freeConn(hconn[cur], cur);
	LOGD(0, "Time taken : ", timeTaken);
	//shutdown(cSock, SHUT_WR);
	while(strlen(tcpRead(cSock)) > 0) usleep(500);
	//close(cSock);

	free(host);
	return maxConn;
}
