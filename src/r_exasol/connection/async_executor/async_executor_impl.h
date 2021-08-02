#ifndef R_EXASOL_ASYNC_EXECUTOR_IMPL_H
#define R_EXASOL_ASYNC_EXECUTOR_IMPL_H

#include <r_exasol/connection/async_executor/async_executor.h>
#include <thread>
#include <atomic>

namespace exa {
    /**
     * Implements the @interface AsyncExecutor using std::thread.
     */
    template<typename TQueryExecutor, typename TSessionInfo>
    class AsyncExecutorImpl : public AsyncExecutor {
    public:
        AsyncExecutorImpl() = default;
        explicit AsyncExecutorImpl(TSessionInfo sessionInfo) : mQueryExecutor(sessionInfo) {};


        /**
         * Allocates the SQL statement and starts the bg thread.
         * @throws OdbcException if the statement allocation fails.
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void execute(exa::tBackgroundAsyncErrorFunction errorHandler) override {
            mQueryExecutor.initializeQueryExecutor();
            mDone = false;
            mThread = std::thread(&AsyncExecutorImpl::executeAsyncQuery, this, errorHandler);
        }

        /**
         * Indicates if data transfer has finished.
         * @return true if data transfer has finished.
         */
        bool isDone() const override {
            return mDone;
        }

        /**
         * Blocks current thread until the bg thread has joined.
         * Please note that in case of the writer the socket connection needs to be closed before joining the bg thread.
         * After the background thread has joined it check a possible error message from the native ODBC API.
         * @return Empty string if no error occured. Error message otherwise.
         */
        std::string joinAndCheckResult() override {
            if (mThread.joinable()) {
                mThread.join();
            }
            return mQueryExecutor.getQueryExecutorResult();
        }

    private:
        /**
         * Executes the remote execution in a bg thread. If an error occurs it calls errorHandler.
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void executeAsyncQuery(exa::tBackgroundAsyncErrorFunction errorHandler) {
            const bool result = mQueryExecutor.executeAsyncQuery();
            mDone = true;
            if (!result) {
                errorHandler();
            }
        }

    private:
        std::atomic_bool mDone{};
        std::thread mThread;
        TQueryExecutor mQueryExecutor;
    };
}

#endif //R_EXASOL_ASYNC_EXECUTOR_IMPL_H
