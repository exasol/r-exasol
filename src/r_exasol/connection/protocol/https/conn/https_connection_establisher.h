
#ifndef R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
#define R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H

#include <r_exasol/connection/connection_establisher.h>


namespace exa {
    class HttpsConnectionEstablisher : public ConnectionEstablisher {
    public:
        virtual ConnectionInfo connect(const char *host, uint16_t port);

    };
}


#endif //R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
