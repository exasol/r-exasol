#include "catch2/catch.hpp"

#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/connection_controller.h>
#include <iostream>
#include "test_utils.h"
#include "mocks/CustomAsyncSessionMock.h"


/**
 * In case of reader test, the server (here the Python program) closes the connection. Thus we need to
 * tell the asynchronous executor via IPC that the connection finished.
 */
TEST_CASE( "ReaderCloseConnection", "[async]" ) {

    exa::ConnectionFactoryImpl factory;
    bool errorWasCalled = false;
    //Instantiate controller and declare error callback as a lambda.
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
    const bool retVal = connectionController.connect(exa::ProtocolType::http, test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getProxyHost() == "Test");
    REQUIRE(connectionController.getProxyPort() == 4);
    //Create our async mock which uses the std::thread implementation.
    //-> reading from cin will not block the main thread immediately,
    // only after the ConnectionController calls shutdown (which will try to join the bg thread).
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";
        return retVal;});
    //Create reader instance
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl);
    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();

    //Create test data
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString(); //Returns 220byte test string

    //Read first 100 bytes of data from remote (Python program) and compare content
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);

    //Read next 100 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);

    //Read last 20 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);

    //Read terminating 0
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);

    //Shutdown controller. Here we join with the background thread
    //If the server would not finish bg execution we would block here
    //(in our case if Python would not send "Finish" via pipe).
    const bool done = connectionController.shutDown();

    //async connection must be finished
    REQUIRE(done);

    //No error is supposed to happen
    REQUIRE(!errorWasCalled);
}

/**
 * In case of reader test, the server (here the Python program) closes the connection. Thus we need to
 * tell the asynchronous executor via IPC that the connection finished. Here we abort the connection before transfer has finished.
 */
TEST_CASE( "ReaderCloseConnectionAbort", "[async]" ) {

    bool errorWasCalled = false;
    exa::ConnectionFactoryImpl factory;
    //Instantiate controller and declare error callback as a lambda.
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
    const bool retVal = connectionController.connect(exa::ProtocolType::http, test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getProxyHost() == "Test");
    REQUIRE(connectionController.getProxyPort() == 4);
    //Create our async mock which uses the std::thread implementation.
    //-> reading from cin will not block the main thread immediately,
    // only after the ConnectionController calls shutdown (which will try to join the bg thread).
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED"; //Actually in this test the server will send abort. But we want to return false here.
        return retVal;});

    //Create reader instance
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl);
    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();

    //Create test data
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString(); //returns 220 bytes of test data

    //Read first 100 bytes of data from remote (Python program) and compare content
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);

    //Read next 100 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);

    //Read last 20 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);

    //Read terminating 0
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);

    //Shutdown controller. Here we join with the background thread
    //If the server would not finish bg execution we would block here
    //(in our case if Python would not send "Abort" via pipe).
    const bool done = connectionController.shutDown(); //Note that done is still 1 if asynchronous SQL was aborted.

    //async connection must be finished
    REQUIRE(done);

    //Error callback must haven been invoked because server sent Abort
    REQUIRE(errorWasCalled);
}

/**
 * In case of writer test, the client (we) closes the connection.
 * We assume here that the server behaves correctly (here the Python server sends us the command via stdin)
 */
TEST_CASE( "WriterCloseConnection", "[async]" ) {

    exa::ConnectionFactoryImpl factory;
    bool errorWasCalled = false;
    //Instantiate controller and declare error callback as a lambda.
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string &error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });

    //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
    const bool retVal = connectionController.connect(exa::ProtocolType::http, test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getProxyHost() == "Test");
    REQUIRE(connectionController.getProxyPort() == 4);


    //Create our async mock which uses the std::thread implementation.
    //-> reading from cin will not block the main thread immediately,
    // only after the ConnectionController calls shutdown (which will try to join the bg thread).
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";
        return retVal;});

    //Create writer instance
    std::weak_ptr<exa::writer::Writer> writer_weak = connectionController.startWriting(mockSessionImpl);
    REQUIRE(!writer_weak.expired());
    auto writer = writer_weak.lock();

    //Write 2 lines of test data:
    // "a"
    // "b"
    std::string data = "\"a\"";
    size_t sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\n";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\"b\"";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\n";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    writer->pipe_fflush();

    //Shutdown controller. Here we join with the background thread
    //If the server would not finish bg execution we would block here
    // (in our case if Python would not send "Finish" via pipe).
    const bool done = connectionController.shutDown();

    //No error is supposed to happen in this test
    REQUIRE(!errorWasCalled);

    //Assume that data transfer as finished.
    REQUIRE(done);
}

/**
 * In case of writer test, the client (we) closes the connection.
 * We assume here that the server behaves correctly (here the Python server sends us the command via stdin)
 * If an error occurs on server side, we assume that the error function will be called.
 */
TEST_CASE( "WriterCloseConnectionAbort", "[async]" ) {
    exa::ConnectionFactoryImpl factory;
    bool errorWasCalled = false;
    //Instantiate controller and declare error callback as a lambda.
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string &error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });

    //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
    const bool retVal = connectionController.connect(exa::ProtocolType::http, test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getProxyHost() == "Test");
    REQUIRE(connectionController.getProxyPort() == 4);


    //Create our async mock which uses the std::thread implementation.
    //-> reading from cin will not block the main thread immediately,
    // only after the ConnectionController calls shutdown (which will try to join the bg thread).
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";//Actually in this test the server will send abort. But we want to return false here.
        return retVal;});

    //Create writer instance
    std::weak_ptr<exa::writer::Writer> writer_weak = connectionController.startWriting(mockSessionImpl);
    REQUIRE(!writer_weak.expired());
    auto writer = writer_weak.lock();

    //Write 2 lines of test data:
    // "a"
    // "b"
    std::string data = "\"a\"";
    size_t sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\n";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\"b\"";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    data = "\n";
    sizeWritten = writer->pipe_write(&(data[0]), 1, data.size());
    REQUIRE(sizeWritten == data.size());
    writer->pipe_fflush();

    //Shutdown controller. Here we join with the background thread
    //If the server would not finish bg execution we would block here
    // (in our case if Python would not send "Abort" via pipe).
    const bool done = connectionController.shutDown();

    //Error callback must haven been invoked because server sent Abort
    REQUIRE(errorWasCalled);

    //Assume that data transfer as finished.
    REQUIRE(done);
}
