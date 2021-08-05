#ifndef R_EXASOL_QUERY_EXECUTOR_H
#define R_EXASOL_QUERY_EXECUTOR_H

#include <string>

namespace exa {
    /**
     * Abstract interface for a query executor, which is used by @class AsyncExecutorImpl to run the query.
     */
    class QueryExecutor {
    public:
        virtual ~QueryExecutor() = default;
        /**
         * Initializes the query executor, called within the main thread.
        * @throws AsyncExecutorException is there occurs an error during initialization.
        */
        virtual void initializeQueryExecutor() = 0;

        /**
         * Returns the result of the query execution. Called on the main thread.
         * @return An empty string if no error occured, the error message otherwise.
         */
        virtual std::string getQueryExecutorResult() = 0;

        /**
         * Executes the query on a background thread.
         * @return true if the execution was successful, false otherwise.
         */
        virtual bool executeAsyncQuery() = 0;

    };
}
#endif //R_EXASOL_QUERY_EXECUTOR_H
