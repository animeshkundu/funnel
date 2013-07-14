#ifndef __CONST_H_
#define __CONST_H_

#define MAXCONN 	50
#define MINCONN 	1
#define THOLD 		1
#define TIMEOUT 	180			/* Default connection timeout. */

#define PORT		4321		/* Service PORT. */
#define SSL_PORT	443
#define TCP_PORT	80
#define MAXSTRUCT	4096

#define READSIZE	4096
#define BUFFERSIZE	40960
#define MINSLEEP	500			/* Time is in micro seconds. */
#define REFRESHIV	50000		/* Also in micro seconds. VERY CRITICAL TO THE PERFORMANCE OF THE CODE.*/

#define LEVEL		6
#define PROD		0			/* Change this to 1 on production build. */

#endif
