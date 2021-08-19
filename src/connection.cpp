#include <connection.h>
#include <r_exasol/connection_context.h>

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


int initConnection(const char* host, int port, const char* protocol) {
    return exa::getConnectionContext().initConnection(host, port, protocol);
}

int destroyConnection(int checkWasDone) {
    return exa::getConnectionContext().destroyConnection(1 == checkWasDone);
}

SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query, const char * protocol) {
    return exa::getConnectionContext().createReadConnection(handle, query, protocol);
}

SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query, const char * protocol) {
    return exa::getConnectionContext().createWriteConnection(handle, query, protocol);
}

SEXP copyHostName() {
    return exa::getConnectionContext().copyHostName();
}

SEXP copyHostPort() {
    return exa::getConnectionContext().copyHostPort();
}

SEXP createCertificate() {
    return exa::getConnectionContext().createCertificate();
}

}
