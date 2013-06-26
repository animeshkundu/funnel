#include "conn.h"
#include "jsmn.h"

#ifndef __HCONN_H_
#define __HCONN_H_

jsmn_parser jsmnP;
jsmntok_t jsmnTok[1000];
int jsmnR, jsmnCount;

char *getToken(const char *);
int handleRequest(int, conn *, int);

#endif
