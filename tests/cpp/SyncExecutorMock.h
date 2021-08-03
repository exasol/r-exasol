#ifndef R_EXASOL_SYNCEXECUTORMOCK_H
#define R_EXASOL_SYNCEXECUTORMOCK_H

#include <r_exasol/connection/async_executor/async_executor.h>

class SyncExecutorMock : public exa::AsyncExecutor {
public:
    explicit SyncExecutorMock(bool & joinCalled);
    void execute(exa::tBackgroundAsyncErrorFunction errorHandler) override;
    bool isDone() const override;
    std::string joinAndCheckResult() override;
private:
    bool & _joinCalled;
};


#endif //R_EXASOL_SYNCEXECUTORMOCK_H
