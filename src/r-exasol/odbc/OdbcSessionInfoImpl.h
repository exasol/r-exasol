#ifndef R_EXASOL_ODBCSESSIONINFOIMPL_H
#define R_EXASOL_ODBCSESSIONINFOIMPL_H

#include <r-exasol/connection/OdbcSessionInfo.h>
#include <sql.h>
#include <r-exasol/odbc/RODBC.h>

namespace exa {
    /**
     * Implementation of the OdbcSessionInfo which stores the ODBCHandle and Query.
     */
    struct OdbcSessionInfoImpl : public OdbcSessionInfo {
        OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query);
        /**
         * Creates an instance of @class OdbcAsyncExecutorImpl.
         * @return  instance of @class OdbcAsyncExecutorImpl.
         */
        std::unique_ptr<OdbcAsyncExecutor> createOdbcAsyncExecutor() const override;
        /**
         * External Odbc handle.
         */
        pRODBCHandle mHandle;
        /**
         * Query string.
         */
        SQLCHAR *mQuery;
    };
}

#endif //R_EXASOL_ODBCSESSIONINFOIMPL_H
