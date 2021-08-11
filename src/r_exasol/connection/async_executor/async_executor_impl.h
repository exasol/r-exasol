#ifndef R_EXASOL_ASYNC_EXECUTOR_IMPL_H
#define R_EXASOL_ASYNC_EXECUTOR_IMPL_H

#include <r_exasol/connection/async_executor/async_executor.h>
#include <r_exasol/debug_print/debug_printer.h>
#include <thread>
#include <atomic>

namespace exa {
    /**
     * Implements the @interface AsyncExecutor using std::thread.
     * @tparam TQueryExecutor must implement interface QueryExecutor. This type will be instantiated as member variable
     *         and used to execute queries in the background thread.
     */
    template<typename TQueryExecutor, typename TSessionInfo>
    class AsyncExecutorImpl : public AsyncExecutor {
        typedef exa::DebugPrinter<exa::AsyncExecutor> ae_debug_printer;
        typedef exa::StackTraceLogger<exa::AsyncExecutor> ae_stack_trace_logger;

    public:
        AsyncExecutorImpl() = default;
        explicit AsyncExecutorImpl(TSessionInfo sessionInfo) : mQueryExecutor(sessionInfo) {};


        /**
         * Initializes the query executor and starts the bg thread.
         * @throws AsyncExecutorException if the statement allocation fails.
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void execute(exa::tBackgroundAsyncErrorFunction errorHandler) override {
            ae_stack_trace_logger astl("execute");
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
         * After the background thread has joined it check a possible error message from @tparam TQueryExecutor.
         * @return Empty string if no error occured. Error message otherwise.
         */
        std::string joinAndCheckResult() override {
            ae_stack_trace_logger astl("join");
            if (mThread.joinable()) {
                ae_debug_printer::print("is joinable!. Call join()");
                mThread.join();
                ae_debug_printer::print("Finished join!");
            }
            return mQueryExecutor.getQueryExecutorResult();
        }

    private:
        /**
         * Executes the remote execution in a bg thread. If an error occurs it calls errorHandler.
         * <p>
         * Do not call any R functions (at least not such which use R's memory management) here!
         * <p>
         * @param errorHandler Callback function to invoke if an error occurs.
         */
        void executeAsyncQuery(exa::tBackgroundAsyncErrorFunction errorHandler) {
            ae_stack_trace_logger astl("executeAsyncQuery");
            const bool result = mQueryExecutor.executeAsyncQuery();
            mDone = true;
            if (!result) {
                errorHandler();
            }
        }

    private:
        ObjectLifecycleLogger<AsyncExecutorImpl> mObjectLifecycleLogger;
        std::atomic_bool mDone{};
        std::thread mThread;
        TQueryExecutor mQueryExecutor;
    };
}

#endif //R_EXASOL_ASYNC_EXECUTOR_IMPL_H
