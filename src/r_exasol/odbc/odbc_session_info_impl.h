#ifndef R_EXASOL_ODBC_SESSION_INFO_IMPL_H
#define R_EXASOL_ODBC_SESSION_INFO_IMPL_H

#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <r_exasol/external/r_odbc_fwd.h>
#include <r_exasol/external/sql.h>

namespace exa {
    /**
     * Implementation of the AsyncExecutorSessionInfo which stores the ODBCHandle and Query.
     */
    struct OdbcSessionInfoImpl : public exa::AsyncExecutorSessionInfo {
        OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query);
        /**
         * Creates an instance of @class OdbcAsyncExecutorImpl.
         * @return  instance of @class OdbcAsyncExecutorImpl.
         */
        std::unique_ptr<AsyncExecutor> createAsyncExecutor() const override;
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
