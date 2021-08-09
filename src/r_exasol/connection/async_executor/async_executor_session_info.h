#ifndef R_EXASOL_ASYNC_EXECUTOR_SESSION_INFO_H
#define R_EXASOL_ASYNC_EXECUTOR_SESSION_INFO_H

#include <memory>

namespace exa {
    class AsyncExecutor;

    /**
     * Abstract interface to the asynchrnous remote session info. The concrete class might contain detailed information
     * about the ODBC handle/queries etc. It provides instantiation of a @class AsyncExecutor.
     */
    struct AsyncExecutorSessionInfo {
        virtual ~AsyncExecutorSessionInfo() = default;
        virtual std::unique_ptr<AsyncExecutor> createAsyncExecutor() const = 0;
    };
}
#endif //R_EXASOL_ASYNC_EXECUTOR_SESSION_INFO_H
