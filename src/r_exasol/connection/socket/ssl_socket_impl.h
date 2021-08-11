#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <r_exasol/ssl/certificate.h>
#include <r_exasol/connection/socket/socket.h>
#include <r_exasol/debug_print/debug_printer.h>

#ifndef R_EXASOL_SSL_SOCKET_IMPL_H
#define R_EXASOL_SSL_SOCKET_IMPL_H

namespace exa {

    class SocketImpl;

    class SSLSocketImpl : public Socket {
    public:
        explicit SSLSocketImpl(SocketImpl & socket, const ssl::Certificate & certificate);
        ~SSLSocketImpl() override;
        ssize_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

    private:
        ObjectLifecycleLogger<SocketImpl> mLifecycleLogger;
        SSL_CTX * mCtx;
        SSL* mSsl;
        const ssl::Certificate & mCertificate;
    };
}


#endif //R_EXASOL_SSL_SOCKET_IMPL_H
