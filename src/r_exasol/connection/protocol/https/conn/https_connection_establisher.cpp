#include "https_connection_establisher.h"

#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/socket/ssl_socket_impl.h>

exa::ConnectionInfo exa::HttpsConnectionEstablisher::connect(const char *host, uint16_t port) {
    SocketImpl socket;
    socket.connect(host, port);
    std::pair<std::string, uint16_t> hostInfo = metaInfoReader::read(socket, host, port);
    std::shared_ptr<SSLSocketImpl> sslSocket = std::make_shared<SSLSocketImpl>(socket);
    ConnectionInfo connectionInfo = {
            .proxyHost = hostInfo.first,
            .proxyPort = hostInfo.second,
            .socket = sslSocket
    };
    return connectionInfo;
}
