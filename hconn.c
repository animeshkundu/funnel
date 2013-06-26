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

	return data;
}

int handleRequest(int cSock, conn *hconn, int maxConn) {
	int cur, sockNo, port;
	connection *sock;
	char *request = NULL, *response, *host, *data, *tmp;

	request = (char *) malloc (2048 * sizeof(char));
	LOG(0, "Inside handleRequest.");
	request = tcpRead(cSock);
	LOGV(0, "Request : ", request);
	while(tmp = getToken(request)) {
		if(tmp == NULL) break;
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
		hconn = realloc(hconn, sizeof(conn));
		hconn[maxConn] = newConn(host, port);
		cur = maxConn; maxConn++;
	}
	
	sockNo = getConn(hconn[cur]);
	LOGD(0, "Socket No : ", sockNo);
	sock = hconn[cur].connPool[sockNo];
	LOG(0, "Got Socket ");
	sslWrite(sock, data);
	LOG(0, "Request sent to Bank.");
	response = sslRead(sock);
	LOGV(0, "Response : ", response);
	tcpWrite(cSock, response);
	LOG(0, "Sent response to client.");
	
	return maxConn;
}
