#ifndef R_EXASOL_ODBC_SESSION_INFO_IMPL_H
#define R_EXASOL_ODBC_SESSION_INFO_IMPL_H

#include <r-exasol/connection/odbc_session_info.h>
#include <r-exasol/odbc/r_odbc_fwd.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sql.h>

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
