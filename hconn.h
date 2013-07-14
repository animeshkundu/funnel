#ifndef __HCONN_H_
#define __HCONN_H_ 

#include "conn.h"
#include "jsmn.h"
#include "sig.h"

static __thread jsmn_parser jsmnP;
static __thread jsmntok_t jsmnTok[MAXSTRUCT];
static __thread int jsmnR, jsmnCount;

static __thread int freeSock, freeCsock;
static __thread conn freeStruct;

int getToken(const char *, char[]);
int handleRequest(int, conn *, int);
int processRead(char[]);
int reconnect(conn, int);

#endif
