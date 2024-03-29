#include "https_connection_establisher.h"

#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/socket/ssl_socket_impl.h>
#include <r_exasol/connection/connection_exception.h>
#include <r_exasol/ssl/certificate_exception.h>

exa::ConnectionInfo exa::HttpsConnectionEstablisher::connect(const char *host, uint16_t port) {
    SocketImpl socket;
    if (!mCertificate.isValid()) {
        throw ConnectionException("certificate has not been initialized.");
    }
    socket.connect(host, port);
    std::pair<std::string, uint16_t> hostInfo = metaInfoReader::read(socket, host, port);
    std::shared_ptr<SSLSocketImpl> sslSocket;
    try {
    sslSocket = std::make_shared<SSLSocketImpl>(socket, mCertificate);
    } catch (const exa::ssl::CertificateException& ex) {
        throw ConnectionException(std::string("Error using certificate for connection:") + ex.what());
    }
    ConnectionInfo connectionInfo = {
            hostInfo.first,
            hostInfo.second,
            sslSocket
    };
    return connectionInfo;
}

exa::HttpsConnectionEstablisher::HttpsConnectionEstablisher(const exa::ssl::Certificate & certificate)
: mCertificate(certificate) {}
