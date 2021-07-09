//
// Created by thomas on 09/07/2021.
//

#ifndef R_EXASOL_EXATYPES_H
#define R_EXASOL_EXATYPES_H

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

#endif //R_EXASOL_EXATYPES_H
