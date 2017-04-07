#ifndef __public__h__h
#define __public__h__h

#include <sys/types.h>          /* basic system data types */
#include <sys/socket.h>         /* basic socket definitions */
#include <sys/time.h>           /* timeval{} for select() */
#include <sys/select.h>         /* select()               */
#include <poll.h>               /* poll()                 */
#include <sys/epoll.h>              /* epoll()                */
#include <sys/stropts.h>
#include <time.h>               /* timespec{} for pselect() */
#include <netinet/in.h>         /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>          /* inet(3) functions */
#include <errno.h>
#include <fcntl.h>              /* for nonblocking */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#define SEVER_PORT 8997
#define MAX_LINE 256

extern ssize_t readn(int filedes, void *buff, size_t nbytes);
extern ssize_t writen(int filedes, const void *buff, size_t nbytes);
extern ssize_t readline(int filedes, void *buff, size_t maxlen);

const long MaxOpen = 1024;
const long MaxEpoll = 1024;
//template<typename T>
inline long max(const long a, const long b)
{
	return a > b ? a: b;
}

typedef void Sigfunc(int);

Sigfunc * signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;
	memset(&act, 0x0, sizeof(act));
	memset(&oact, 0x0, sizeof(oact));
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}else{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if(sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}

#endif

