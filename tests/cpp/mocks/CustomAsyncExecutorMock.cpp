#include "CustomAsyncExecutorMock.h"
#include "CustomAsyncSessionMock.h"

CustomAsyncExecutorMock::CustomAsyncExecutorMock(const CustomAsyncSessionMock & customAsyncSessionMock)
: asyncFunction(customAsyncSessionMock.asyncFunction)
, asyncFunctionWasSuccessful(false) {}

bool CustomAsyncExecutorMock::executeAsyncQuery() {
    asyncFunctionWasSuccessful = asyncFunction();
    return asyncFunctionWasSuccessful;
}

void CustomAsyncExecutorMock::initializeQueryExecutor() {}

std::string CustomAsyncExecutorMock::getQueryExecutorResult() {
    return asyncFunctionWasSuccessful ? std::string() : std::string("Async Error");
}
