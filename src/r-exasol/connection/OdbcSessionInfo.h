#ifndef R_EXASOL_ODBCSESSIONINFO_H
#define R_EXASOL_ODBCSESSIONINFO_H

#include <memory>

namespace exa {
    class OdbcAsyncExecutor;

    /**
     * Abstract interface to the ODBC session info. The concrete class might contain detailed information
     * about the ODBC handle/queries etc. It provides instantiaon of a @class OdbcAsyncExecutor.
     */
    struct OdbcSessionInfo {
        virtual ~OdbcSessionInfo() = default;
        virtual std::unique_ptr<OdbcAsyncExecutor> createOdbcAsyncExecutor() const = 0;
    };
}
#endif //R_EXASOL_ODBCSESSIONINFO_H
