#ifndef R_EXASOL_CONNECTION_CONTEXT_H
#define R_EXASOL_CONNECTION_CONTEXT_H

#include <r_exasol/external/r.h>
#include <r_exasol/external/sql.h>
#include <r_exasol/external/r_odbc_fwd.h>
#include <memory>
#include <r_exasol/rconnection/r_connection.h>
#include <r_exasol/connection/connection_controller.h>
#include <r_exasol/connection/connection_factory_impl.h>

namespace exa {

    /**
     * This class provides the same interface as the interface C <-> R, on the C++ level.
     * It instantiates the R-connection class, Connection Controller and the Factory for the Connection Controller.
     */
    class ConnectionContext {
    public:

        int initConnection(const char* host, int port, const char* protocol);
        SEXP copyHostName();
        SEXP copyHostPort();
        int destroyConnection(bool checkDone);
        SEXP createReadConnection(::pRODBCHandle handle, ::SQLCHAR *query, const char* protocol);
        SEXP createWriteConnection(::pRODBCHandle handle, ::SQLCHAR *query, const char* protocol);

    private:
        exa::ProtocolType convertProtocol(const char*);

    private:
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <exa::ConnectionController> mConnectionController;
        exa::ConnectionFactoryImpl mConnectionFactory;
    };
}


#endif //R_EXASOL_CONNECTION_CONTEXT_H
