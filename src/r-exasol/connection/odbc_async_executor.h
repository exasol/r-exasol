#ifndef R_EXASOL_ODBC_ASYNC_EXECUTOR_H
#define R_EXASOL_ODBC_ASYNC_EXECUTOR_H

#include <r-exasol/connection/error_handler.h>

namespace exa {
    /**
     * Abstract interface which used by the @class ConnectionController to trigger the asynchronous ODBC statement execution.
     */
    class OdbcAsyncExecutor {
    public:
        virtual ~OdbcAsyncExecutor() = default;
        /**
         * Starts the asynchronous ODBC statement execution. The function returns immediately (before the statement execution has been finished).
         * @param errorHandler Reference to a error callback function which shall be invoked if an error occurs during the asynchronous operation.
         */
        virtual void execute(tBackgroundOdbcErrorFunction errorHandler) = 0;
        /**
         * Indicates if the asynchronous ODBC statement execution has been finished.
         * @return true if the asynchronous ODBC statement execution has been finished, false otherwise.
         */
        virtual bool isDone() = 0;

        /**
         * Blocks until the background execution has been finished, and returns the result.
         * @return An empty string if no error occured. The error description otherwise.
         */
        virtual std::string joinAndCheckResult() = 0;
    };
}
#endif //R_EXASOL_ODBC_ASYNC_EXECUTOR_H
