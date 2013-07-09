#include "conn.h"
#include "jsmn.h"

#ifndef __HCONN_H_
#define __HCONN_H_ 1

jsmn_parser jsmnP;
jsmntok_t jsmnTok[1000];
int jsmnR, jsmnCount;

int getToken(const char *, char[]);
int handleRequest(int, conn *, int);
int processRead(char[]);
int reconnect(conn, int);

#endif
