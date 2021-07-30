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


int initConnection(const char* host, int port) {
    return exa::getConnectionContext().initConnection(host, port);
}

int destroyConnection(int closeFd) {
    return exa::getConnectionContext().destroyConnection(closeFd);
}

SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query) {
    return exa::getConnectionContext().createReadConnection(handle, query);
}

SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query) {
    return exa::getConnectionContext().createWriteConnection(handle, query);
}

extern SEXP copyHostName() {
    return exa::getConnectionContext().copyHostName();
}

extern SEXP copyHostPort() {
    return exa::getConnectionContext().copyHostPort();
}

}
