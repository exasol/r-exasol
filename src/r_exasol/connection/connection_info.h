#ifndef R_EXASOL_CONNECTION_INFO_H
#define R_EXASOL_CONNECTION_INFO_H

#include <r_exasol/connection/socket/socket.h>
#include <string>
#include <memory>

namespace exa {
    struct ConnectionInfo {
        std::string proxyHost;
        uint16_t proxyPort;
        std::shared_ptr<Socket> socket;
    };
}
#endif //R_EXASOL_CONNECTION_INFO_H
