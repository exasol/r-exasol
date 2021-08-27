#ifndef R_EXASOL_CERTIFICATE_EXCEPTION_H
#define R_EXASOL_CERTIFICATE_EXCEPTION_H

#include <stdexcept>

namespace exa {
    namespace ssl {
        class CertificateException : public std::runtime_error {
            using std::runtime_error::runtime_error;
        };
    }
}
#endif //R_EXASOL_CERTIFICATE_EXCEPTION_H
