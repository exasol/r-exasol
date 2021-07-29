#ifndef R_EXASOL_HTTP_COMMON_H
#define R_EXASOL_HTTP_COMMON_H

#include <r-exasol/connection/socket/Socket.h>

namespace exa {
    /**
     * Reads the http header from the given socket.
     * @param s Socket which is supposed to be open, from which the http header will be read.
     */
    void readHttpHeader(Socket& s);
}
#endif