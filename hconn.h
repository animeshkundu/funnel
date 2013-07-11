#ifndef __HCONN_H_
#define __HCONN_H_ 1

#include "conn.h"
#include "jsmn.h"

jsmn_parser jsmnP;
jsmntok_t jsmnTok[1000];
static __thread int jsmnR, jsmnCount;
static __thread int freeSock, freeCsock;
static __thread conn freeStruct;

int getToken(const char *, char[]);
int handleRequest(int, conn *, int);
int processRead(char[]);
int reconnect(conn, int);

#endif
