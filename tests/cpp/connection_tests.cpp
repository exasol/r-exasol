#include "catch2/catch.hpp"

#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/connection_controller.h>
#include <iostream>
#include "test_utils.h"
#include "mocks/AsyncSessionMock.h"


/**
 * Purpose of this test is to verify behavior of HttpChunkReader:
 * 1. Testing reading of the HttpHeader
 * 2. Testing correct reading of the Http payload (chunk), by comparing content (@see test_utils::createTestString())
 */
TEST_CASE( "ImportHttp", "[connection]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(test_utils::host, test_utils::PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
    reader->start();
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
    socket->shutdownRdWr();
}


/**
 * Purpose of this test is to verify behavior of ConnectionController:
 * 1. Test correct reading of the metadata (remote host + port)
 * 2. Creation of reader object
 * 3. Testing reading of the HttpHeader
 * 4. Testing correct reading of the Http payload (chunk), by comparing content (@see test_utils::createTestString())
 * 5. Correct behavior of the asynchronous executor during shutdown (calling join on std::thread()).
 */
TEST_CASE( "ConnectionControllerImport", "[connection]" ) {
    bool errorCalled = false;
    bool joinCalled = false;
    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [&errorCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorCalled = true;
    });
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    AsyncSessionMock mockSessionImpl(joinCalled);
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
    connectionController.shutDown();
    REQUIRE(!errorCalled);
}

/**
 * Purpose of this test is to verify if we can successful read data after writing to server,
 * with other words we test if no garbage remains in memory after one cycle, and that we can instatiate a clean
 * reaer object after we used a writer object.
 *
 */
TEST_CASE( "ConnectionControllerEcho", "[connection]" ) {

    exa::ConnectionFactoryImpl factory;
    SECTION( "first testing writing to server" )
    {
        bool errorCalled = false;
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });
        bool joinCalled(false);
        const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);

        REQUIRE(retVal);
        REQUIRE(connectionController.getHostInfo().first == "Test");
        REQUIRE(connectionController.getHostInfo().second == 4);
        AsyncSessionMock mockSessionImpl(joinCalled);
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
        connectionController.shutDown();
        REQUIRE(joinCalled);
        REQUIRE(!errorCalled);
    }
    SECTION( "now testing reading from server" )
    {
        bool errorCalled = false;
        bool joinCalled(false);
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });
        const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);
        REQUIRE(retVal);
        REQUIRE(connectionController.getHostInfo().first == "Test");
        REQUIRE(connectionController.getHostInfo().second == 4);
        AsyncSessionMock mockSessionImpl(joinCalled);
        std::weak_ptr<exa::reader::Reader> readerWeak =
                connectionController.startReading(mockSessionImpl, exa::ProtocolType::http);
        REQUIRE(!readerWeak.expired());
        auto reader = readerWeak.lock();
        std::stringstream data;
        do {
            const int c = reader->fgetc();
            if (-1 == c) break;
            data << static_cast<char>(c);
        } while (true);

        REQUIRE(std::string("Name\na\nb") == data.str());
        connectionController.shutDown();
        REQUIRE(joinCalled);
        REQUIRE(!errorCalled);
    }
}

/**
 * Purpose of this test is to simulate an error during import.
 * The server (python program) aborts the connections after receiving metadata.
 * We check that the controller correctly returns "false" in this case.
 * In the second step we verify that another attempts of running the import works correctly.
 */
TEST_CASE( "ConnectionControllerImportWithError", "[connection]" ) {
    exa::ConnectionFactoryImpl factory;
    SECTION( "testing first error case" )
    {
        bool errorCalled = false;
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });
        const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);

        REQUIRE(!retVal);
        REQUIRE(errorCalled);
        connectionController.shutDown();
    }
    SECTION( "now testing success case" )
    {
        bool errorCalled = false;
        exa::ConnectionController newConnectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });
        const bool retVal = newConnectionController.connect(test_utils::host, test_utils::PORT);
        bool joinCalled(false);
        REQUIRE(retVal);
        REQUIRE(newConnectionController.getHostInfo().first == "Test");
        REQUIRE(newConnectionController.getHostInfo().second == 4);
        AsyncSessionMock mockSessionImpl(joinCalled);
        std::weak_ptr<exa::reader::Reader> readerWeak =
                newConnectionController.startReading(mockSessionImpl, exa::ProtocolType::http);
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
        newConnectionController.shutDown();
        REQUIRE(joinCalled);
        REQUIRE(!errorCalled);
    }
}
