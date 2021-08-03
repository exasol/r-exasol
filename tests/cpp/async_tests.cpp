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
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";
        return retVal;});
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl, exa::ProtocolType::http);

    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString();
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);
    const bool done = connectionController.shutDown();
    REQUIRE(done);
    REQUIRE(!errorWasCalled);
}

/**
 * In case of reader test, the server (here the Python program) closes the connection. Thus we need to
 * tell the asynchronous executor via IPC that the connection finished. Here we abort the connection before transfer has finished.
 */
TEST_CASE( "ReaderCloseConnectionAbort", "[async]" ) {

    bool errorWasCalled = false;
    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED"; //Actually in this test the server will send abort. But we want to return false here.
        return retVal;});
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl, exa::ProtocolType::http);

    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString();
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);
    const bool done = connectionController.shutDown(); //Note that done is still 1 if asynchronous SQL was aborted.
    REQUIRE(done);
    REQUIRE(errorWasCalled);
}

/**
 * In case of writer test, the client (we) closes the connection. We assume here that the server behaves correctly (here the Python server sends us the command via stdin)
 */
TEST_CASE( "WriterCloseConnection", "[async]" ) {

    exa::ConnectionFactoryImpl factory;
    bool errorWasCalled = false;
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string &error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);

    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";
        return retVal;});
    std::weak_ptr<exa::writer::Writer> writer_weak = connectionController.startWriting(mockSessionImpl, exa::ProtocolType::http);
    REQUIRE(!writer_weak.expired());
    auto writer = writer_weak.lock();
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
    const bool done = connectionController.shutDown();
    REQUIRE(!errorWasCalled);
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
    exa::ConnectionController connectionController(factory, [&errorWasCalled](const std::string &error) {
        std::cout << "ERROR:" << error << std::endl;
        errorWasCalled = true;
    });
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);

    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    CustomAsyncSessionMock mockSessionImpl([]() {
        std::string lock; std::cin >> lock;
        const bool retVal = lock == "FINISHED";//Actually in this test the server will send abort. But we want to return false here.
        return retVal;});
    std::weak_ptr<exa::writer::Writer> writer_weak = connectionController.startWriting(mockSessionImpl, exa::ProtocolType::http);
    REQUIRE(!writer_weak.expired());
    auto writer = writer_weak.lock();
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
    const bool done = connectionController.shutDown();
    REQUIRE(errorWasCalled);
    REQUIRE(done);
}
