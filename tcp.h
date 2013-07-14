#ifndef __TCP_H_
#define __TCP_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "const.h"
#include "log.h"

int tcpCreate(int);
int tcpConnect (char *, int);
int tcpRead (int, char[]);
int tcpWrite(int, char *);
void tcpDisconnect(int);

#endif
