#ifndef R_EXASOL_ASYNC_EXECUTOR_EXCEPTION_H
#define R_EXASOL_ASYNC_EXECUTOR_EXCEPTION_H

#include <stdexcept>

namespace exa {
    /**
     * An exception which might occur starting the asynchronous remote statenent execution.
     */
    class AsyncExecutorException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}


#endif //R_EXASOL_ASYNC_EXECUTOR_EXCEPTION_H
