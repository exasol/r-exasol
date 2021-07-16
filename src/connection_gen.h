//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_CONNECTION_GEN_H
#define R_EXASOL_CONNECTION_GEN_H

#include <Rdefines.h>
#include <if/ExaTypes.h>
#include <sql.h>
#include "odbc/RODBC.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern int initConnection(const char* host, int port);
    extern SEXP copyHostName();
    extern SEXP copyHostPort();
    extern int destroyConnection(int closeFd);
    extern SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query);
    extern SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query);

#ifdef __cplusplus
}
#endif

#endif //R_EXASOL_CONNECTION_GEN_H
