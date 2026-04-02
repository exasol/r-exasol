#include <connection.h>
#include <r_exasol/connection_context.h>
#include <r_exasol/debug_print/file_logger.h>
#include <r_exasol/websocket/exasol_commands.h>

namespace exa {
    /*
     * Store ConnectionContext as a singleton. Thus it allocates memory only
     * at the first time a connection is trying to be established, and not
     * when the library is loaded.
     */
    ConnectionContext & getConnectionContext() {
        static ConnectionContext connectionContext;
        return connectionContext;
    }
}


extern "C" {

int enableTracing(const char* tracefile) {
    return exa::getConnectionContext().enableTracing(tracefile);
}

int initConnection(const char* host, int port, const char* protocol) {
    return exa::getConnectionContext().initConnection(host, port, protocol);
}

int destroyConnection(int checkWasDone) {
    return exa::getConnectionContext().destroyConnection(1 == checkWasDone);
}

SEXP copyHostName() {
    return exa::getConnectionContext().copyHostName();
}

SEXP copyHostPort() {
    return exa::getConnectionContext().copyHostPort();
}

SEXP createReadConnectionWs(void *wsSession, const char *query, const char *protocol) {
    auto *cmds = static_cast<exa::ExasolCommands*>(wsSession);
    return exa::getConnectionContext().createReadConnectionWs(cmds, query, protocol);
}

SEXP createWriteConnectionWs(void *wsSession, const char *query, const char *protocol) {
    auto *cmds = static_cast<exa::ExasolCommands*>(wsSession);
    return exa::getConnectionContext().createWriteConnectionWs(cmds, query, protocol);
}

}
