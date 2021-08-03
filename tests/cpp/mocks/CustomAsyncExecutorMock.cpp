#include "CustomAsyncExecutorMock.h"
#include "CustomAsyncSessionMock.h"

CustomAsyncExecutorMock::CustomAsyncExecutorMock(const CustomAsyncSessionMock & customAsyncSessionMock)
: asyncFunction(customAsyncSessionMock.asyncFunction) {}

bool CustomAsyncExecutorMock::executeAsyncQuery() {
    return asyncFunction();
}

void CustomAsyncExecutorMock::initializeQueryExecutor() {}

std::string CustomAsyncExecutorMock::getQueryExecutorResult() {
    return {};
}
