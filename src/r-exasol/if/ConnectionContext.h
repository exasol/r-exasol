//
// Created by thomas on 27/07/2021.
//

#ifndef R_EXASOL_CONNECTIONCONTEXT_H
#define R_EXASOL_CONNECTIONCONTEXT_H

#include <Rdefines.h>
#include <r-exasol/if/ExaTypes.h>
#include <sql.h>
#include <r-exasol/impl/odbc/RODBC.h>

#include <r-exasol/impl/connection/ConnectionFactoryImpl.h>
#include <r-exasol/impl/rconnection/RConnection.h>
#include <r-exasol/impl/connection/ConnectionController.h>

namespace exa {

    class ConnectionContext {
    public:
        ConnectionContext() = default;

        int initConnection(const char* host, int port);
        SEXP copyHostName();
        SEXP copyHostPort();
        int destroyConnection(int closeFd);
        SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query);
        SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query);

    private:
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <ConnectionController> mConnectionController;
        ConnectionFactoryImpl mConnectionFactory;
    };
}


#endif //R_EXASOL_CONNECTIONCONTEXT_H
