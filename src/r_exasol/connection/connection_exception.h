#include <stdexcept>

#ifndef R_EXASOL_CONNECTIONEXCEPTION_H
#define R_EXASOL_CONNECTIONEXCEPTION_H

namespace exa {
    /**
     * Connection specific exception (can be thrown by protocols).
     */
    class ConnectionException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

#endif //R_EXASOL_CONNECTIONEXCEPTION_H
