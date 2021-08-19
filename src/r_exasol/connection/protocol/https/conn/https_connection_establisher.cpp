#include "https_connection_establisher.h"

#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/socket/ssl_socket_impl.h>
#include <r_exasol/connection/connection_exception.h>
#include <r_exasol/ssl/certificate_exception.h>

exa::ConnectionInfo exa::HttpsConnectionEstablisher::connect(const char *host, uint16_t port,
                                                             const ssl::Certificate& certificate) {
    SocketImpl socket;
    if (!certificate.isValid()) {
        throw ConnectionException("certificate has not been initialized.");
    }
    socket.connect(host, port);
    std::pair<std::string, uint16_t> hostInfo = metaInfoReader::read(socket, host, port);
    std::shared_ptr<SSLSocketImpl> sslSocket;
    try {
    sslSocket = std::make_shared<SSLSocketImpl>(socket, certificate);
    } catch (const exa::ssl::CertificateException& ex) {
        throw ConnectionException(std::string("Error using certificate for connection:") + ex.what());
    }
    ConnectionInfo connectionInfo = {
            .proxyHost = hostInfo.first,
            .proxyPort = hostInfo.second,
            .socket = sslSocket
    };
    return connectionInfo;
}
