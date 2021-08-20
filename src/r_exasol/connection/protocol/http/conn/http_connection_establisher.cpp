#include "http_connection_establisher.h"
#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/protocol/meta_info_reader.h>

exa::ConnectionInfo exa::HttpConnectionEstablisher::connect(const char *host, uint16_t port) {
    std::shared_ptr<SocketImpl> socket = std::make_shared<SocketImpl>();
    socket->connect(host, port);
    std::pair<std::string, uint16_t> hostInfo = metaInfoReader::read(*socket, host, port);
    return {
        hostInfo.first,
        hostInfo.second,
        socket
    };
}
