#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Windows version
#include <winsock2.h>
#include <windows.h>
#define WINDOWS

extern char* program_name;

#define INIT()          init( argv );
#define EXIT(s)         do { WSACleanup(); exit( ( s ) ); } while ( 0 )
#define CLOSE(s)        if( closesocket( s ) ) error( 1, errno, "Error while closing socket")
#define set_errno(e)    SetLastError( ( e ) )
#define isvalidsock(s)  ( ( s ) != SOCKET_ERROR )
#define bzero(b,n)      memset( ( b ), 0, ( n ) )
#define sleep(t)        Sleep( (t) * 1000 )

#ifdef __cplusplus
}
#endif
