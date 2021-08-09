#ifndef R_EXASOL_ASYNCEXECUTORMOCK_H
#define R_EXASOL_ASYNCEXECUTORMOCK_H

#include <string>

class AsyncSessionMock;
class AsyncExecutorMock {
public:
    explicit AsyncExecutorMock(AsyncSessionMock &session);
    void initializeQueryExecutor();
    std::string getQueryExecutorResult();
    bool executeAsyncQuery();

private:
    bool & joinCalled;
};


#endif //R_EXASOL_ASYNCEXECUTORMOCK_H
