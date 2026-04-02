#ifndef R_EXASOL_CONNECTION_CONTEXT_H
#define R_EXASOL_CONNECTION_CONTEXT_H

#include <r_exasol/external/r.h>
#include <memory>
#include <r_exasol/rconnection/r_connection.h>
#include <r_exasol/connection/connection_controller.h>
#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/debug_print/file_logger.h>

namespace exa {
    class ExasolCommands;

    /**
     * This class provides the same interface as the interface C <-> R, on the C++ level.
     * It instantiates the R-connection class, Connection Controller and the Factory for the Connection Controller.
     */
    class ConnectionContext {
    public:

        ConnectionContext();
        int initConnection(const char* host, int port, const char* protocol);
        SEXP copyHostName();
        SEXP copyHostPort();
        int destroyConnection(bool checkDone);
        SEXP createReadConnectionWs(ExasolCommands* cmds, const char* query, const char* protocol);
        SEXP createWriteConnectionWs(ExasolCommands* cmds, const char* query, const char* protocol);
        int enableTracing(const char* tracefile);
    private:
        static exa::ProtocolType convertProtocol(const char*);


    private:
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <exa::ConnectionController> mConnectionController;
        exa::ConnectionFactoryImpl mConnectionFactory;
        exa::debug::FileLogger mLogger;
    };
}


#endif //R_EXASOL_CONNECTION_CONTEXT_H
