#ifndef R_EXASOL_CONNECTION_CONTEXT_H
#define R_EXASOL_CONNECTION_CONTEXT_H

#include <Rdefines.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sql.h>
#include <r-exasol/odbc/r_odbc.h>
#include <memory>
#include <r-exasol/rconnection/r_connection.h>
#include <r-exasol/connection/connection_controller.h>
#include <r-exasol/connection/connection_factory_impl.h>

namespace exa {

    /**
     * This class provides the same interface as the interface C <-> R, on the C++ level.
     * It instantiates the R-connection class, Connection Controller and the Factory for the Connection Controller.
     */
    class ConnectionContext {
    public:

        int initConnection(const char* host, int port);
        SEXP copyHostName();
        SEXP copyHostPort();
        int destroyConnection(int closeFd);
        SEXP createReadConnection(::pRODBCHandle handle, ::SQLCHAR *query);
        SEXP createWriteConnection(::pRODBCHandle handle, ::SQLCHAR *query);

    private:
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <exa::ConnectionController> mConnectionController;
        exa::ConnectionFactoryImpl mConnectionFactory;
    };
}


#endif //R_EXASOL_CONNECTION_CONTEXT_H
