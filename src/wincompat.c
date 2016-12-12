#include "skel.h"
#ifdef WINDOWS

#define MINBSDSOCKERR   ( WSAEWOULDBLOCK )
#define MAXBSDSOCKERR   ( MINBSDSOCKERR + \
                        ( sizeof( bsdsocketerrs ) / \
                        sizeof( bsdsocketerrs[ 0 ] ) ) )

//extern int sys_nerr;
//extern char* sys_errlist[];
extern char* program_name;
static char* bsdsocketerrs[] =
{
    (char *) "Resource temporarily unavailable",
    (char *) "Operation now in progress",
    (char *) "Operation already in progress",
    (char *) "Socket operation on non-socket",
    (char *) "Destination address required",
    (char *) "Message too long",
    (char *) "Protocol wrong type for socket",
    (char *) "Bad protocol option",
    (char *) "Protocol not supported",
    (char *) "Socket type not supported",
    (char *) "Operation not supported",
    (char *) "Protocol family not supported",
    (char *) "Address family not supported by protocol family",
    (char *) "Address already in use",
    (char *) "Can't assign requested address",
    (char *) "Network is down",
    (char *) "Network is unreachable",
    (char *) "Network dropped connection on reset",
    (char *) "Software caused connection abort",
    (char *) "Connection reset by peer",
    (char *) "No buffer space available",
    (char *) "Socket is already connected",
    (char *) "Socket is not connected",
    (char *) "Cannot send after socket shutdown",
    (char *) "Too many references: can't splice",
    (char *) "Connection timed out",
    (char *) "Connection refused",
    (char *) "Too many levels of symbolic links",
    (char *) "File name too long",
    (char *) "Host is down",
    (char *) "No route to host"
};

void init(char **argv) {
    WSADATA wsadata;
    ( program_name = strrchr(argv[0], '\\') ) ?
            program_name++ : ( program_name = argv[0] );
    WSAStartup( MAKEWORD(2,2), &wsadata );
}

/* inet_aton - версия inet_aton для SVr4 и Windows */
int inet_aton(char *cp, struct in_addr* pin) {
    u_long rc; //int?

    rc = inet_addr(cp);
    if(rc == -1 && strcmp(cp, "255.255.255.255")) return 0;
    pin->s_addr = rc;
    return 1;
}

/* strerror - версия, включающая код ошибок Winsock */
char* strerror(int err) {
    if(err >= 0 && err < sys_nerr)
        return sys_errlist[err];
    else if (err >= MINBSDSOCKERR && err < MAXBSDSOCKERR)
        return bsdsocketerrs[err - MINBSDSOCKERR];
    else if (err == WSASYSNOTREADY)
        return (char *) "Network subsystem is unusable";
    else if (err == WSAVERNOTSUPPORTED)
        return (char *) "This version of Winsock is not supported";
    else if (err == WSANOTINITIALISED)
        return (char *) "Winsock is not initialized";
    else
        return (char *) "Unknown error";
}
#endif