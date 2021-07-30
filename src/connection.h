/**
 * This module serves as bridge between the plain C code and C++ code.
 * It simply forwards all requests to @class ConnectionContext.
 */

#ifndef R_EXASOL_CONNECTION_H
#define R_EXASOL_CONNECTION_H

#include <r_exasol/external/r.h>
#include <r_exasol/external/sql.h>
#include <r_exasol/external/r_odbc_fwd.h>

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

#endif //R_EXASOL_CONNECTION_H
