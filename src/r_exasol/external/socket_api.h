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
//SOCKET SHUT_RDWR
//See https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
//Send: "If the how parameter is SD_SEND, subsequent calls to the send function are disallowed. For TCP sockets, a FIN will be sent after all data is sent and acknowledged by the receiver."
//Receive: "If the how parameter is SD_RECEIVE, subsequent calls to the recv function on the socket will be disallowed. This has no effect on the lower protocol layers. For TCP sockets, if there is still data queued on the socket waiting to be received, or data arrives subsequently, the connection is reset, since the data cannot be delivered to the user. For UDP sockets, incoming datagrams are accepted and queued. In no case will an ICMP error packet be generated."
//SD_BOTH corresponds to Linux SHUT_RDWR
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
//SOCKET SHUT_WR
//See https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
//"If the how parameter is SD_SEND, subsequent calls to the send function are disallowed. For TCP sockets, a FIN will be sent after all data is sent and acknowledged by the receiver."
//SD_SEND corresponds to Linux SHUT_WR
#ifndef SHUT_WR
#define SHUT_WR SD_SEND
#endif

#endif

#endif //R_EXASOL_SOCKET_API_H
