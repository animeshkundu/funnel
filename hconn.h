#include "conn.h"
#include "jsmn.h"

#ifndef __hconn_h_
#define __hconn_h_

jsmn_parser jsmnP;
jsmntok_t jsmnTok[1000];
int jsmnR, jsmnCount = 0;

#endif
