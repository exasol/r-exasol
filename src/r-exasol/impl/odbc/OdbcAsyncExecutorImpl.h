//
// Created by thomas on 15/07/2021.
//

#ifndef R_EXASOL_ODBCASYNCEXECUTORIMPL_H
#define R_EXASOL_ODBCASYNCEXECUTORIMPL_H

#include <sql.h>
#include <r-exasol/impl/odbc/RODBC.h>
#include <r-exasol/if/OdbcAsyncExecutor.h>
#include <r-exasol/impl/odbc/OdbcSessionInfoImpl.h>
#include <thread>
#include <atomic>

namespace exa {
    class OdbcAsyncExecutorImpl : public OdbcAsyncExecutor {
    public:
        explicit OdbcAsyncExecutorImpl(OdbcSessionInfoImpl );
        ~OdbcAsyncExecutorImpl() override;

        bool execute(tBackgroundOdbcErrorFunction errorHandler) override;
        bool isDone() override;
        std::string joinAndCheckResult() override;
    private:
        void asyncRODBCQueryExecuter(tBackgroundOdbcErrorFunction errorHandler);

    private:
        const OdbcSessionInfoImpl mOdbcSessionInfo;
        SQLHSTMT mStmt;
        std::atomic_bool mDone{};
        std::thread mThread;
        SQLRETURN mRes;
    };
}

#endif //R_EXASOL_ODBCASYNCEXECUTORIMPL_H
