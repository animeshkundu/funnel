#ifndef __LOG_H_
#define __LOG_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "exceptions.h"
#include "log.h"

/* This function ain't gonna cut it on production. */
/* TODO: Write an elaborate logger. Support for both file log and syslog would be helpful. */

/* Don't want logging on PROD. */
#define LOG(n, msg) if(n >= LEVEL && !PROD) printf("\n[ %s ] - [ %s : %s : %d ]\t%s\n", __TIME__, __FILE__,__func__, __LINE__, msg)
#define LOGD(n, msg, var) if(n >= LEVEL && !PROD) printf("\n[ %s ] - [ %s : %s : %d ]\t%s %d\n", __TIME__, __FILE__,__func__, __LINE__, msg, var)
#define LOGV(n, msg, var) if(n >= LEVEL && !PROD) printf("\n[ %s ] - [ %s : %s : %d ]\t%s %s\n", __TIME__, __FILE__,__func__, __LINE__, msg, var)

#endif
