//
// Created by thomas on 26/07/2021.
//

#ifndef R_EXASOL_ODBCEXCEPTION_H
#define R_EXASOL_ODBCEXCEPTION_H

namespace exa {
    class OdbcException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}


#endif //R_EXASOL_ODBCEXCEPTION_H
