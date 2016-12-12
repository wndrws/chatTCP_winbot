#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "skel.h"

#define NLISTEN 5   // Максимальное число ожидающих соединений

#ifdef __SVR4
#define bzero(b,n) memset( ( b ), 0, ( n ) )
#endif

//typedef void ( *tofunc_t )(void *);

void error(int, int, const char *, ...);
int readn(SOCKET, char *, int);
int readvrec(SOCKET, char *, int);
int readline(SOCKET, char*, int);
SOCKET tcp_server(char *, char *);
SOCKET tcp_client(char *, char *);
//int udp_server(char*, char*);
//int udp_client(char*, char*, struct sockaddr_in*);
static void set_address(char *, char *, struct sockaddr_in *, char *);
int inet_aton(char *cp, struct in_addr* pin);
void init(char **argv);

#ifdef __cplusplus
}
#endif