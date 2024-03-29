#ifndef R_EXASOL_CERTIFICATE_H
#define R_EXASOL_CERTIFICATE_H

#include <openssl/x509.h>

namespace exa {
    namespace ssl {
        class Certificate {
        public:
            Certificate() = default;
            Certificate(const Certificate&) = delete;
            Certificate& operator=(const Certificate&) = delete;
            ~Certificate();
            void mkcert(int bits = 2048, int serial = 0, int days = 365);
            void apply(SSL_CTX *ctx) const;
            bool isValid() const;
        private:
            X509 *mX509p = nullptr;
            EVP_PKEY *mPkeyp = nullptr;
        };
    }
}


#endif //R_EXASOL_CERTIFICATE_H
