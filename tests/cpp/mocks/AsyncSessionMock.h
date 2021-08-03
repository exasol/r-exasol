#ifndef R_EXASOL_ASYNCSESSIONMOCK_H
#define R_EXASOL_ASYNCSESSIONMOCK_H

#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <r_exasol/connection/async_executor/async_executor.h>

struct AsyncSessionMock : exa::AsyncExecutorSessionInfo {
    explicit AsyncSessionMock(bool & joinCalled);

    std::unique_ptr<exa::AsyncExecutor> createAsyncExecutor() const override;
    bool & joinCalled;
};

#endif //R_EXASOL_ASYNCSESSIONMOCK_H
