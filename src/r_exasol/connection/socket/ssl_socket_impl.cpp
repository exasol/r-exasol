#include <r_exasol/connection/socket/ssl_socket_impl.h>
#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/algorithm/repeating_buffer_fill.h>
#include <r_exasol/external/socket_api.h>
#include <limits>

//Check https://opensource.apple.com/source/OpenSSL/OpenSSL-22/openssl/demos/ssl/serv.cpp.auto.html


#define STACK_PRINTER StackTraceLogger<exa::SSLSocketImpl> stackTraceLogger(__func__)


ssize_t exa::SSLSocketImpl::recv(void *buf, size_t len) {
    STACK_PRINTER;
    ssize_t retVal = -1;
    if (len <= static_cast<size_t>(std::numeric_limits<ssize_t>::max())) {
        //SSL works with packets of length 16KB. So we might need to call SSL_read several time to read the whole buffer.
        char* buffer = static_cast<char*>(buf);
        using namespace std::placeholders;
        auto sslRead = std::bind(::SSL_read, mSsl,  _1, _2);
        bool success =
                exa::algo::repeatingBufferFill<char*>(buffer, buffer + len, sslRead);
        retVal = success ? static_cast<ssize_t>(len) : -1;
    }
    return retVal;
}

ssize_t exa::SSLSocketImpl::send(const void *buf, size_t len) {
    STACK_PRINTER;
    //::SSL_write will return only after whole chunk was written
    //unless SSL_MODE_ENABLE_PARTIAL_WRITE has been set. However, per default this mode is disabled.
    return ::SSL_write(mSsl, buf, len);
}

void exa::SSLSocketImpl::shutdownWr() {
    STACK_PRINTER;
    ::SSL_shutdown(mSsl);
}

void exa::SSLSocketImpl::shutdownRdWr() {
    STACK_PRINTER;
    ::SSL_shutdown(mSsl);
}

exa::SSLSocketImpl::SSLSocketImpl(SocketImpl & socket, const ssl::Certificate & certificate)
: mCertificate(certificate) {
    ::SSL_load_error_strings();
    ::SSLeay_add_ssl_algorithms();
    mCtx = ::SSL_CTX_new(SSLv23_server_method());

    mCertificate.apply(mCtx);
    mSsl = ::SSL_new(mCtx);
    ::SSL_set_mode(mSsl, SSL_MODE_AUTO_RETRY);
    ::SSL_set_fd(mSsl, socket.detach());
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

