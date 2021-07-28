//
// Created by thomas on 15/07/2021.
//

#ifndef R_EXASOL_ODBCASYNCEXECUTOR_H
#define R_EXASOL_ODBCASYNCEXECUTOR_H

#include <r-exasol/connection/ErrorHandler.h>

namespace exa {
    class OdbcAsyncExecutor {
    public:
        virtual ~OdbcAsyncExecutor() = default;
        virtual bool execute(tBackgroundOdbcErrorFunction errorHandler) = 0;
        virtual bool isDone() = 0;
        virtual std::string joinAndCheckResult() = 0;
    };
}
#endif //R_EXASOL_ODBCASYNCEXECUTOR_H
