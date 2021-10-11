#ifndef R_EXASOL_SOCKET_API_H
#define R_EXASOL_SOCKET_API_H


#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <netdb.h>

#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
#ifndef SHUT_WR
#define SHUT_WR SD_SEND
#endif

#endif

#endif //R_EXASOL_SOCKET_API_H
