#ifndef R_EXASOL_ODBC_QUERY_EXECUTOR_H
#define R_EXASOL_ODBC_QUERY_EXECUTOR_H

#include <r_exasol/external/r_odbc.h>

#include <r_exasol/odbc/odbc_session_info_impl.h>
#include <thread>
#include <atomic>


namespace exa {
    /**
     * Implements the functions used by @class AsyncExecutorImpl using the native ODBC functions.
     * For the reader connection note:
     *  The server will close the socket connection and finishes the the ODBC execution.
     * For the writer connection note:
     *  The client will close the socket connection when the data stream has finished -> This will trigger the server to finish the ODBC execution.
     */
    class OdbcQueryExecutor {
    public:
        explicit OdbcQueryExecutor(OdbcSessionInfoImpl );
        ~OdbcQueryExecutor(); //not marked virtual by purpose, we don't expect inherited classes

        /**
         * Initializes the ODBC query executor, allocating the SQL statement.
         * @throws AsyncExecutorException is there occurs an error during the allocation of the SQL statement.
         */
        void initializeQueryExecutor();

        /**
         * Returns the result of the ODBC execution.
         * @return An empty string if no error occured, the error message otherwise.
         */
        std::string getQueryExecutorResult();

        /**
         * Executes the ODBC statement.
         * @return true if the execution was successful, false otherwise.
         */
        bool executeAsyncQuery();

    private:
        const OdbcSessionInfoImpl mOdbcSessionInfo;
        ::SQLHSTMT mStmt;
        ::SQLRETURN mRes;
    };
}

#endif //R_EXASOL_ODBC_QUERY_EXECUTOR_H
