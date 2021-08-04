#include "catch2/catch.hpp"

#include <r_exasol/connection//protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer//http_chunk_writer.h>
#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <iostream>
#include "test_utils.h"

/**
 * Import small test data and read buffer-wise from server.
 * This test also will compare content received from server.
 */
TEST_CASE( "Import", "[http]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(test_utils::host, test_utils::PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
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
 * Export small test data and write buffer-wise to server.
 * Server (python script) will compare content.
 */
TEST_CASE( "Export", "[http]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(test_utils::host, test_utils::PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::writer::HttpChunkWriter> writer = std::make_unique<exa::writer::HttpChunkWriter>(socket, chunk);
    std::string testString = test_utils::createTestString();
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


/**
 * Purpose of this test is to transfer several chunks between server and client and
 * check if there is no memory issue (buffer overflow etc.) when working with data
 * not fitting into one chunk. (Tests should run with asan enabled).
 * We don't compare the content of the data here.
 */
TEST_CASE( "ImportBig", "[http]" ) {
    std::shared_ptr<exa::Socket> socket = std::make_shared<exa::SocketImpl>();
    socket->connect(test_utils::host, test_utils::PORT);
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString();
    int charReceived = 0;
    int idxChunk = 0;
    do {
        charReceived = reader->fgetc();
    } while(charReceived >= 0);

    socket->shutdownRdWr();
}
