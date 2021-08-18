#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <r_exasol/connection/socket/ssl/certificate.h>
#include <r_exasol/connection/socket/socket.h>


#ifndef R_EXASOL_SSL_SOCKET_IMPL_H
#define R_EXASOL_SSL_SOCKET_IMPL_H

namespace exa {

    class SocketImpl;

    class SSLSocketImpl : public Socket {
    public:
        explicit SSLSocketImpl(SocketImpl & socket);
        ~SSLSocketImpl() override;
        size_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

    private:

        SSL_CTX * mCtx;
        SSL* mSsl;
        ssl::Certificate mCertificate;
    };
}


#endif //R_EXASOL_SSL_SOCKET_IMPL_H
