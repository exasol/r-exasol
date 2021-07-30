#ifndef R_EXASOL_ODBCASYNCEXECUTORIMPL_H
#define R_EXASOL_ODBCASYNCEXECUTORIMPL_H

#include <r-exasol/odbc/RODBC.h>

#include <r-exasol/connection/OdbcAsyncExecutor.h>
#include <r-exasol/odbc/OdbcSessionInfoImpl.h>
#include <thread>
#include <atomic>


namespace exa {
    /**
     * Implements the @interface OdbcAsyncExecutor using the native ODBC functions.
     * The statement handle is allocated in the main thread, the statement execution happens in a bg thread (using std::thread).
     * For the reader connection note:
     *  The server will close the socket connection and finishes the the ODBC execution.
     * For the writer connection note:
     *  The client will close the socket connection when the data stream has finished -> This will trigger the server to finish the ODBC execution.
     */
    class OdbcAsyncExecutorImpl : public OdbcAsyncExecutor {
    public:
        explicit OdbcAsyncExecutorImpl(OdbcSessionInfoImpl );
        ~OdbcAsyncExecutorImpl() override;

        /**
         * Allocates the SQL statement and starts the bg thread.
         * @throws OdbcException if the statement allocation fails.
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void execute(exa::tBackgroundOdbcErrorFunction errorHandler) override;

        /**
         * Indicates if data transfer has finished.
         * @return true if data transfer has finished.
         */
        bool isDone() override;

        /**
         * Blocks current thread until the bg thread has joined.
         * Please note that in case of the writer the socket connection needs to be closed before joining the bg thread.
         * After the background thread has joined it check a possible error message from the native ODBC API.
         * @return Empty string if no error occured. Error message otherwise.
         */
        std::string joinAndCheckResult() override;
    private:
        /**
         * Executes the @private mStmt in a bg thread. If an error occurs it calls errorHandler.
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void asyncRODBCQueryExecuter(exa::tBackgroundOdbcErrorFunction errorHandler);

    private:
        const OdbcSessionInfoImpl mOdbcSessionInfo;
        ::SQLHSTMT mStmt;
        std::atomic_bool mDone{};
        std::thread mThread;
        ::SQLRETURN mRes;
    };
}

#endif //R_EXASOL_ODBCASYNCEXECUTORIMPL_H
