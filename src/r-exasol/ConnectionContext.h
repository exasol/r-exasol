//
// Created by thomas on 27/07/2021.
//

#ifndef R_EXASOL_CONNECTIONCONTEXT_H
#define R_EXASOL_CONNECTIONCONTEXT_H

#include <Rdefines.h>
#include <r-exasol/connection/ExaTypes.h>
#include <sql.h>
#include <r-exasol/odbc/RODBC.h>
#include <memory>
#include <r-exasol/rconnection/RConnection.h>
#include <r-exasol/connection/ConnectionController.h>
#include <r-exasol/connection/ConnectionFactoryImpl.h>

namespace exa {

    class ConnectionContext {
    public:

        int initConnection(const char* host, int port);
        SEXP copyHostName();
        SEXP copyHostPort();
        int destroyConnection(int closeFd);
        SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query);
        SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query);

    private:
        std::unique_ptr <rconnection::RConnection> mConnection;
        std::unique_ptr <ConnectionController> mConnectionController;
        ConnectionFactoryImpl mConnectionFactory;
    };
}


#endif //R_EXASOL_CONNECTIONCONTEXT_H
