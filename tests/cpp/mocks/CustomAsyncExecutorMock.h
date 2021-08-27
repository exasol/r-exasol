#ifndef R_EXASOL_CUSTOMASYNCEXECUTORMOCK_H
#define R_EXASOL_CUSTOMASYNCEXECUTORMOCK_H

#include <r_exasol/connection/async_executor/async_executor.h>

#include "CustomMockFunction.h"

struct CustomAsyncSessionMock;
class CustomAsyncExecutorMock {
public:
    explicit CustomAsyncExecutorMock(const CustomAsyncSessionMock & customAsyncSessionMock);
    void initializeQueryExecutor();
    std::string getQueryExecutorResult();
    bool executeAsyncQuery();

private:
    tAsyncFunction asyncFunction;
    bool asyncFunctionWasSuccessful;
};

#endif //R_EXASOL_CUSTOMASYNCEXECUTORMOCK_H
