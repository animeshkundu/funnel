#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#ifndef __TCP_H_
#define __TCP_H_

#define LEVEL 5
#define LOG(n, msg) if(n >= LEVEL) printf("\n[ %s ] - [ %s : %s : %d ]\t%s\n", __TIME__, __FILE__,__func__, __LINE__, msg)
#define LOGD(n, msg, var) if(n >= LEVEL) printf("\n[ %s ] - [ %s : %s : %d ]\t%s %d\n", __TIME__, __FILE__,__func__, __LINE__, msg, var)
#define LOGV(n, msg, var) if(n >= LEVEL) printf("\n[ %s ] - [ %s : %s : %d ]\t%s %s\n", __TIME__, __FILE__,__func__, __LINE__, msg, var)

int error(char *);
int tcpCreate(int);
int tcpConnect (char *, int);
char * tcpRead (int);
void tcpWrite(int, char *);

#endif
