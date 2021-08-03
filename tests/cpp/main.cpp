#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include <r_exasol/connection//protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer//http_chunk_writer.h>
#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/connection_controller.h>
#include <sstream>
#include <r_exasol/connection/async_executor/async_executor_impl.h>

#include "AsyncSessionMock.h"
#include "SyncSessionMock.h"

static const char host[] = "localhost";
const int PORT = 5000;


std::string createTestString() {
    std::ostringstream os;
    std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
    return os.str();
}

struct MyListener : Catch::TestEventListenerBase {

    using TestEventListenerBase::TestEventListenerBase; // inherit constructor

    void testCaseStarting( Catch::TestCaseInfo const& testInfo ) override {
#ifdef _WIN32
        _sleep(1);
#else
        ::sleep(1);
#endif
    }

    void testCaseEnded( Catch::TestCaseStats const& testCaseStats ) override {
        // Tear-down after a test case is run
    }
};

CATCH_REGISTER_LISTENER( MyListener )

TEST_CASE( "Import", "[reader]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
    std::vector<char> buffer(100);
    std::string testString = createTestString();
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


TEST_CASE( "ImportHttp", "[reader]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
    reader->start();
    std::vector<char> buffer(100);
    std::string testString = createTestString();
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


TEST_CASE( "ConnectionControllerImport", "[reader]" ) {
    static bool joinCalled = false;

    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [](const std::string& error) { std::cout << "ERROR:" << error << std::endl;});
    const bool retVal = connectionController.connect(host, PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    AsyncSessionMock mockSessionImpl(joinCalled);
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl, exa::ProtocolType::http);

    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();
    std::vector<char> buffer(100);
    std::string testString = createTestString();
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
}

TEST_CASE( "ConnectionControllerEcho", "[reader/writer]" ) {

    exa::ConnectionFactoryImpl factory;
    SECTION( "first testing writing to server" )
    {
        exa::ConnectionController connectionController(factory, [](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
        });
        bool joinCalled(false);
        const bool retVal = connectionController.connect(host, PORT);

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
    }
    SECTION( "now testing reading from server" )
    {
        bool joinCalled(false);
        exa::ConnectionController connectionController(factory, [](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
        });
        const bool retVal = connectionController.connect(host, PORT);
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
    }
}

TEST_CASE( "ConnectionControllerImportWithError", "[reader]" ) {
    exa::ConnectionFactoryImpl factory;
    SECTION( "testing first error case" )
    {
        exa::ConnectionController connectionController(factory, [](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
        });
        const bool retVal = connectionController.connect(host, PORT);

        REQUIRE(!retVal);
        connectionController.shutDown();
    }
    SECTION( "now testing success case" )
    {
        exa::ConnectionController newConnectionController(factory, [](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
        });
        const bool retVal = newConnectionController.connect(host, PORT);
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
        std::string testString = createTestString();
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

    }
}

TEST_CASE( "Export", "[writer]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::writer::HttpChunkWriter> writer = std::make_unique<exa::writer::HttpChunkWriter>(socket, chunk);
    std::string testString = createTestString();
    const int bufferSize = 100;
    size_t sizeWritten = writer->pipe_write(&(testString[0]), 1, bufferSize);
    REQUIRE(sizeWritten == bufferSize);
    sizeWritten = writer->pipe_write(&(testString[bufferSize]), 1, bufferSize);
    REQUIRE(sizeWritten == bufferSize);
    sizeWritten = writer->pipe_write(&(testString[2*bufferSize]), 1, 20);
    REQUIRE(sizeWritten == 20);
    writer->pipe_fflush();
    socket->shutdownRdWr();
}
