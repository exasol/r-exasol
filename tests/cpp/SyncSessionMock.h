#ifndef R_EXASOL_SYNCSESSIONMOCK_H
#define R_EXASOL_SYNCSESSIONMOCK_H

#include <r_exasol/connection/error_handler.h>
#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <memory>

class SyncSessionMock : public exa::AsyncExecutorSessionInfo {
    explicit  SyncSessionMock(bool & joinCalled);
    std::unique_ptr<exa::AsyncExecutor> createAsyncExecutor() const;
private:
    bool & joinCalled;
};




#endif //R_EXASOL_SYNCSESSIONMOCK_H
