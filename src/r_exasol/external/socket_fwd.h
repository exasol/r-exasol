#ifndef R_EXASOL_EXT_SOCKET_IF_H
#define R_EXASOL_EXT_SOCKET_IF_H


#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
    typedef int tSocket;
#else
    typedef SOCKET tSocket;
#endif

#ifdef __cplusplus
}
#endif



#endif //R_EXASOL_EXT_SOCKET_IF_H
