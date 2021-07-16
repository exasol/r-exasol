//
// Created by thomas on 15/07/2021.
//

#ifndef R_EXASOL_ODBCASYNCEXECUTORIMPL_H
#define R_EXASOL_ODBCASYNCEXECUTORIMPL_H

#include <sql.h>
#include "RODBC.h"
#include <if/OdbcAsyncExecutor.h>
#include "OdbcSessionInfoImpl.h"
#include <thread>
#include <atomic>

namespace exa {
    class OdbcAsyncExecutorImpl : public OdbcAsyncExecutor {
    public:
        explicit OdbcAsyncExecutorImpl(OdbcSessionInfoImpl );
        ~OdbcAsyncExecutorImpl() override;

        bool execute(const tErrorFunction& errorHandler) override;
        bool isDone() override;
        void join() override;
    private:
        void asyncRODBCQueryExecuter(const tErrorFunction& errorHandler);

    private:
        const OdbcSessionInfoImpl mOdbcSessionInfo;
        SQLHSTMT mStmt;
        std::atomic_bool mDone{};
        std::thread mThread;
    };
}

#endif //R_EXASOL_ODBCASYNCEXECUTORIMPL_H
