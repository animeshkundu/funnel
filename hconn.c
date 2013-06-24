#include "hconn.h"

char *getToken(const char * js) {
	if(0 == jsmnCount) {
		jsmn_init(&jsmnP);
		jsmnR = jsmn_parse(&jsmnP, js, jsmnTok, 10);
		if(jsmnR != JSMN_SUCCESS) {
			printf("\nCannot decode Json");
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
	return data;
}

int handleRequest(int cSock, conn *hconn, int maxConn) {
	int cur, sockNo, sock, port;
	char *request, *response, *host, *data, *tmp;

	request = tcpRead(cSock);
	while(tmp = getToken(request)) {
		if(tmp == NULL) break;
		if(!strcmp("host", tmp)) host = getToken(request);
		if(!strcmp("port", tmp)) port = atoi(getToken(request));
		if(!strcmp("data", tmp)) data = getToken(request);
	} 
	
	free(tmp); free(request);

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
