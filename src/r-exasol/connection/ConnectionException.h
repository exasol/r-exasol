//
// Created by thomas on 15/07/2021.
//

#include <stdexcept>

#ifndef R_EXASOL_CONNECTIONEXCEPTION_H
#define R_EXASOL_CONNECTIONEXCEPTION_H

namespace exa {
    class ConnectionException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

#endif //R_EXASOL_CONNECTIONEXCEPTION_H
