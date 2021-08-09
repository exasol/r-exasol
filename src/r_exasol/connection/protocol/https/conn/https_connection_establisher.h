
#ifndef R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
#define R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H

#include <r_exasol/connection/connection_establisher.h>
#include <r_exasol/connection/connection_factory.h>

namespace exa {
    class HttpsConnectionEstablisher : public ConnectionEstablisher {
    public:
        HttpsConnectionEstablisher(exa::ConnectionFactory &connectionFactory);
        virtual ConnectionInfo connect(const char *host, uint16_t port);

    private:
        ConnectionFactory & mConnectionFactory;
    };
}


#endif //R_EXASOL_HTTPS_CONNECTION_ESTABLISHER_H
