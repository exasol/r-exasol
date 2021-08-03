#include "SyncExecutorMock.h"

SyncExecutorMock::SyncExecutorMock(bool & joinCalled)
: _joinCalled(joinCalled) {}

void SyncExecutorMock::execute(exa::tBackgroundAsyncErrorFunction errorHandler) {}

bool SyncExecutorMock::isDone() const {
    return true;
}

std::string SyncExecutorMock::joinAndCheckResult() {
    _joinCalled = true;
    return {};
}
