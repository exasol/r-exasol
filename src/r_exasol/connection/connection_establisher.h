#ifndef R_EXASOL_CONNECTION_ESTABLISHER_H
#define R_EXASOL_CONNECTION_ESTABLISHER_H

#include <string>
#include <r_exasol/connection/connection_info.h>
#include <r_exasol/ssl/certificate.h>

namespace exa {
    class ConnectionEstablisher {
    public:
        virtual ~ConnectionEstablisher() = default;
        virtual ConnectionInfo connect(const char *host, uint16_t port, const ssl::Certificate &) = 0;
    };
}
#endif //R_EXASOL_CONNECTION_ESTABLISHER_H
