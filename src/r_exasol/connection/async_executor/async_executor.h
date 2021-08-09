#ifndef R_EXASOL_ASYNC_EXECUTOR_H
#define R_EXASOL_ASYNC_EXECUTOR_H

#include <r_exasol/connection/error_handler.h>

namespace exa {
    /**
     * Abstract interface which used by the @class ConnectionController to trigger the asynchronous remote statement execution.
     */
    class AsyncExecutor {
    public:
        virtual ~AsyncExecutor() = default;
        /**
         * Starts the asynchronous remote statement execution. The function returns immediately (before the statement execution has been finished).
         * @param errorHandler Reference to a error callback function which shall be invoked if an error occurs during the asynchronous operation.
         */
        virtual void execute(tBackgroundAsyncErrorFunction errorHandler) = 0;
        /**
         * Indicates if the asynchronous ODBC statement execution has been finished.
         * @return true if the asynchronous ODBC statement execution has been finished, false otherwise.
         */
        virtual bool isDone() const = 0;

        /**
         * Blocks until the background execution has been finished, and returns the result.
         * @return An empty string if no error occured. The error description otherwise.
         */
        virtual std::string joinAndCheckResult() = 0;
    };
}
#endif //R_EXASOL_ASYNC_EXECUTOR_H
