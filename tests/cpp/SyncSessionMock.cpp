#include "SyncSessionMock.h"
#include "SyncExecutorMock.h"

SyncSessionMock::SyncSessionMock(bool & joinCalled)
: joinCalled(joinCalled) {}

std::unique_ptr<exa::AsyncExecutor> SyncSessionMock::createAsyncExecutor() const {
    return std::make_unique<SyncExecutorMock>(joinCalled);
}
