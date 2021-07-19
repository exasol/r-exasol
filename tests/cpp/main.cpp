//
// Created by thomas on 09/07/2021.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include <r-exasol/impl/connection//protocol/http/reader/HttpChunkReader.h>
#include <r-exasol/impl/connection/protocol/http/writer//HttpChunkWriter.h>
#include <r-exasol/impl/connection/socket/SocketImpl.h>

#include <dlfcn.h>
#include <r-exasol/impl/connection/ConnectionFactoryImpl.h>
#include <r-exasol/if/ConnectionController.h>

static const char host[] = "localhost";
const int PORT = 5000;

std::string createTestString() {
    std::ostringstream os;
    std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
    return os.str();
}

TEST_CASE( "Import", "[reader]" ) {
    std::unique_ptr<exa::Socket> socket = std::make_unique<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(*socket, chunk);
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
    std::unique_ptr<exa::Socket> socket = std::make_unique<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(*socket, chunk);
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
    class OdbcAsyncExecutorMock : public exa::OdbcAsyncExecutor {
    public:
        virtual bool execute(const exa::tErrorFunction& errorHandler) { return true; }
        virtual bool isDone() {return true;}
        virtual void join() { joinCalled = true; }
    };

    struct OdbcSessionImpl : exa::OdbcSessionInfo {
        std::unique_ptr<exa::OdbcAsyncExecutor> createOdbcAsyncExecutor() const override { return std::make_unique<OdbcAsyncExecutorMock>(); }
    };

    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [](const std::string& error) { std::cout << "ERROR:" << error << std::endl;});
    const int retVal = connectionController.connect(host, PORT);
    REQUIRE(retVal == 0);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    OdbcSessionImpl odbcSessionImpl;
    exa::reader::Reader* reader = connectionController.startReading(odbcSessionImpl, exa::ProtocolType::http);

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
    REQUIRE(joinCalled == true);
}

TEST_CASE( "ConnectionControllerImportWithError", "[reader]" ) {
    static bool joinCalled = false;
    class OdbcAsyncExecutorMock : public exa::OdbcAsyncExecutor {
    public:
        virtual bool execute(const exa::tErrorFunction& errorHandler) { return true; }
        virtual bool isDone() {return true;}
        virtual void join() { joinCalled = true; }
    };

    struct OdbcSessionImpl : exa::OdbcSessionInfo {
        std::unique_ptr<exa::OdbcAsyncExecutor> createOdbcAsyncExecutor() const override { return std::make_unique<OdbcAsyncExecutorMock>(); }
    };

    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [](const std::string& error) { std::cout << "ERROR:" << error << std::endl;});
    int retVal = connectionController.connect(host, PORT);
    REQUIRE(retVal == 0);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    OdbcSessionImpl odbcSessionImpl;
    exa::reader::Reader* reader = connectionController.startReading(odbcSessionImpl, exa::ProtocolType::http);
    std::cerr << "YEAH";
    REQUIRE(reader == nullptr);
    connectionController.shutDown();
    exa::ConnectionController newConnectionController(factory, [](const std::string& error) { std::cout << "ERROR:" << error << std::endl;});
    retVal = newConnectionController.connect(host, PORT);
    REQUIRE(retVal == 0);
    REQUIRE(newConnectionController.getHostInfo().first == "Test");
    REQUIRE(newConnectionController.getHostInfo().second == 4);
    reader = newConnectionController.startReading(odbcSessionImpl, exa::ProtocolType::http);

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
    REQUIRE(joinCalled == true);
}

TEST_CASE( "Export", "[writer]" ) {
    std::unique_ptr<exa::Socket> socket = std::make_unique<exa::SocketImpl>();
    socket->connect(host, PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::writer::HttpChunkWriter> writer = std::make_unique<exa::writer::HttpChunkWriter>(*socket, chunk);
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
