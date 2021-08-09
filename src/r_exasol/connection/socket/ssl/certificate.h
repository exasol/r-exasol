#ifndef R_EXASOL_CERTIFICATE_H
#define R_EXASOL_CERTIFICATE_H

#include <openssl/x509.h>

namespace exa {
    namespace ssl {
        class Certificate {
        public:
            Certificate() = default;
            ~Certificate();
            void mkcert(int bits, int serial, int days);
            void apply(SSL_CTX *ctx);
        private:
            X509 *mX509p = nullptr;
            EVP_PKEY *mPkeyp = nullptr;
        };
    }
}


#endif //R_EXASOL_CERTIFICATE_H
