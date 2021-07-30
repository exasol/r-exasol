#ifndef R_EXASOL_ODBC_SESSION_INFO_IMPL_H
#define R_EXASOL_ODBC_SESSION_INFO_IMPL_H

#include <r_exasol/connection/odbc_session_info.h>
#include <r_exasol/external/r_odbc_fwd.h>
#include <r_exasol/external/sql.h>

namespace exa {
    /**
     * Implementation of the OdbcSessionInfo which stores the ODBCHandle and Query.
     */
    struct OdbcSessionInfoImpl : public exa::OdbcSessionInfo {
        OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query);
        /**
         * Creates an instance of @class OdbcAsyncExecutorImpl.
         * @return  instance of @class OdbcAsyncExecutorImpl.
         */
        std::unique_ptr<OdbcAsyncExecutor> createOdbcAsyncExecutor() const override;
        /**
         * External Odbc handle.
         */
        ::pRODBCHandle mHandle;
        /**
         * Query string.
         */
        ::SQLCHAR *mQuery;
    };
}

#endif //R_EXASOL_ODBC_SESSION_INFO_IMPL_H
