
#ifndef R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
#define R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H

#include <r_exasol/connection/connection_establisher.h>
#include <r_exasol/ssl/certificate.h>

namespace exa {
    class HttpsConnectionEstablisher : public ConnectionEstablisher {
    public:
        explicit HttpsConnectionEstablisher(const exa::ssl::Certificate&);
        ConnectionInfo connect(const char *host, uint16_t port) override;

    private:
        const ssl::Certificate& mCertificate;
    };
}


#endif //R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
