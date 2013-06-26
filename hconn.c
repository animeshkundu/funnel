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

	char * data;
	data = (char *) malloc ( sizeof(char) * (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start + 1) );
	strncpy (data, js+jsmnTok[jsmnCount].start, (jsmnTok[jsmnCount].end - jsmnTok[jsmnCount].start));
	jsmnCount++;

	LOGV(0, "Returing token.", data);
	return data;
}

int handleRequest(int cSock, conn *hconn, int maxConn) {
	int cur, sockNo, sock, port;
	char *request = NULL, *response, *host, *data, *tmp;

	request = (char *) malloc (2048 * sizeof(char));
	LOG(0, "Inside handleRequest.");
	request = tcpRead(cSock);
	LOGV(0, "Request : ", request); LOG(0, "");
	while(tmp = getToken(request)) {
		if(tmp == NULL) break;
		if(!strcmp("host", tmp)) host = getToken(request);
		if(!strcmp("port", tmp)) port = atoi(getToken(request));
		if(!strcmp("data", tmp)) data = getToken(request);
	} 
	
	free(tmp); free(request);
	
	LOGV(0, "Host : ", host);
	LOGV(0, "Port : ", port);
	LOGV(0, "Data : ", data);
	cur = exists(hconn, maxConn, host, port);
	
	if(cur < 0) {
		hconn = realloc(hconn, sizeof(conn));
		hconn[maxConn] = newConn(host, port);
		cur = maxConn; maxConn++;
	}
		
	sockNo = getConn(hconn[cur]);
	sock = hconn[cur].connPool[sockNo];
	sslWrite(sock, data);
	response = sslRead(sock);
	tcpWrite(cSock, response);
	
	return maxConn;
}
