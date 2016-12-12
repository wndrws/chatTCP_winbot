#include <stdint.h>
#include "etcp.h"

SOCKET tcp_server(char* hostname, char* portname) {
    struct sockaddr_in local;
    SOCKET s;
    //const int optval = 1;

    set_address(hostname, portname, &local, "tcp");
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(!isvalidsock(s))
        error(1, errno, "Error in socket() call", strerror(WSAGetLastError()));
    //if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*) &optval, sizeof(optval)));
    //    error(1, errno, "Error in setsockopt() call");
    if(bind(s, (struct sockaddr*) &local, sizeof(local)))
        error(1, errno, "Error in bind() call", strerror(WSAGetLastError()));
    if(listen(s, NLISTEN))
        error(1, errno, "Error in listen() call", strerror(WSAGetLastError()));
    return s;
}

SOCKET tcp_client(char* hostname, char* portname) {
    struct sockaddr_in peer;
    SOCKET s;

    set_address(hostname, portname, &peer, "tcp");
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(!isvalidsock(s))
        error(1, errno, "Error in socket() call", strerror(WSAGetLastError()));
    if(connect(s, (struct sockaddr*) &peer, sizeof(peer)))
        error(1, errno, "Error in connect() call: %s", strerror(WSAGetLastError()));
    return s;
}

static void set_address(char* hostname, char* portname,
                             struct sockaddr_in *sap, char* protocol) {
    struct servent *sp;
    struct hostent *hp;
    char* endptr;
    uint16_t port;

    bzero(sap, sizeof(*sap));
    sap->sin_family = AF_INET;
    if(hostname != NULL) {
        if(!inet_aton(hostname, &sap->sin_addr)) {
            hp = gethostbyname(hostname);
            if(hp == NULL)
                error(1, 0, "Unknown host: %s\n", hostname);
            else sap->sin_addr = *(struct in_addr*)hp->h_addr;
        }
    } else sap->sin_addr.s_addr = htonl(INADDR_ANY);
    port = (uint16_t) strtol(portname, &endptr, 0);
    if(*endptr == '\0') sap->sin_port = htons(port);
    else {
        sp = getservbyname(portname, protocol);
        if(sp == NULL)
            error(1, 0, "Unknown port: %s\n", portname);
        else sap->sin_port = (u_short) sp->s_port;
    }
}