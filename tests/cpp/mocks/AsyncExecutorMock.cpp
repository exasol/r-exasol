#include "AsyncExecutorMock.h"
#include "AsyncSessionMock.h"

AsyncExecutorMock::AsyncExecutorMock(AsyncSessionMock &session) : joinCalled(session.joinCalled) {}

void AsyncExecutorMock::initializeQueryExecutor() {}

std::string AsyncExecutorMock::getQueryExecutorResult() {
    joinCalled = true; return {};
}

bool AsyncExecutorMock::executeAsyncQuery() { return true; }
