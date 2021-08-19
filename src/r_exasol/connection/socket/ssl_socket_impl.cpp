#include <r_exasol/connection/socket/ssl_socket_impl.h>
#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/algorithm/repeating_buffer_fill.h>
#include <r_exasol/external/socket_api.h>

size_t exa::SSLSocketImpl::recv(void *buf, size_t len) {

    //SSL works with packets of length 16KB. So we might need to call SSL_read several time to read the whole buffer.
    char* buffer = static_cast<char*>(buf);
    using namespace std::placeholders;
    auto sslRead = std::bind(::SSL_read, mSsl,  _1, _2);
    bool success =
            exa::algo::repeatingBufferFill<char*>(buffer, buffer + len, sslRead);

    return success ? len : -1;
}

ssize_t exa::SSLSocketImpl::send(const void *buf, size_t len) {
    //::SSL_write will return only after whole chunk was written
    //unless SSL_MODE_ENABLE_PARTIAL_WRITE has been set. However, per default this mode is disabled.
    return ::SSL_write(mSsl, buf, len);
}

void exa::SSLSocketImpl::shutdownWr() {
    ::SSL_shutdown(mSsl);
}

void exa::SSLSocketImpl::shutdownRdWr() {
    ::SSL_shutdown(mSsl);
}

exa::SSLSocketImpl::SSLSocketImpl(SocketImpl & socket, const ssl::Certificate & certificate)
: mCertificate(certificate) {
    ::SSL_load_error_strings();
    ::SSL_library_init();
    ::ERR_load_BIO_strings();
    ::OpenSSL_add_all_algorithms();
    ::SSLeay_add_ssl_algorithms();
    mCtx = ::SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_ecdh_auto(ctx, 1);

    mCertificate.apply(mCtx);
    mSsl = nullptr;
    mSsl = ::SSL_new(mCtx);
    ::SSL_set_mode(mSsl, SSL_MODE_AUTO_RETRY);
    ::SSL_set_fd(mSsl, socket.transferOwnership());
    ::SSL_set_accept_state(mSsl);
}

exa::SSLSocketImpl::~SSLSocketImpl() {
    tSocket s = ::SSL_get_fd(mSsl);
    const int ret = ::SSL_shutdown(mSsl);
    if (0 == ret) {
        ::SSL_shutdown(mSsl);
    }
    ::SSL_free(mSsl);
    ::SSL_CTX_free (mCtx);
    ERR_free_strings ();

    ::shutdown(s, SHUT_RDWR);
}

