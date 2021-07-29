#ifndef R_EXASOL_ODBCEXCEPTION_H
#define R_EXASOL_ODBCEXCEPTION_H

#include <stdexcept>

namespace exa {
    /**
     * An exception which might occur starting the ODBC statenent execution.
     */
    class OdbcException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}


#endif //R_EXASOL_ODBCEXCEPTION_H
