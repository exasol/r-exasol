#include "catch2/catch.hpp"

#include <r_exasol/connection//protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer//http_chunk_writer.h>
#include <r_exasol/connection/connection_factory_impl.h>
#include <iostream>
#include "test_utils.h"


/**
 * Import small test data and read character by character from server.
 * This test also will compare content received from server.
 */
void testImport(std::shared_ptr<exa::Socket> socket) {
    exa::Chunk chunk{};

    std::cerr << "Creating http chunk reader" << std::endl;
    //Create reader
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);

    //Create test data
    std::vector<char> buffer(220);
    std::string testString = test_utils::createTestString(); //returns 220 bytes of test data
    size_t sizeReceived = 0;
    int c = -1;
    std::cerr << "Start import" << std::endl;
    do {
        c = reader->fgetc();
        if (c >= 0) {
            REQUIRE(sizeReceived < buffer.size());
            buffer[sizeReceived] = static_cast<char>(c);
            sizeReceived++;
        }
    } while (c >= 0);

    //Compare what we received.....
    REQUIRE(buffer.size() == sizeReceived);
    REQUIRE(::memcmp(buffer.data(), testString.c_str(), buffer.size()) == 0);
    std::cerr << "Shutdown socket." << std::endl;
    socket->shutdownRdWr();
    std::cerr << "Finished." << std::endl;
}

/**
 * Create a normal socket and test import.
 */
TEST_CASE( "ImportHttp", "[http]" ) {
    testImport(test_utils::createSocket());
}

/**
 * Create a secure socket and test import.
 */
TEST_CASE( "ImportHttps", "[http]" ) {
    testImport(test_utils::createSecureSocket());
}

/**
 * Export small test data and write buffer-wise to server.
 * Server (python script) will compare content.
 */
void testExport(std::shared_ptr<exa::Socket> socket) {
    exa::Chunk chunk{};

    //Create writer
    std::unique_ptr<exa::writer::HttpChunkWriter> writer = std::make_unique<exa::writer::HttpChunkWriter>(socket, chunk);
    std::string testString = test_utils::createTestString(); //returns 220 bytes of test data

    //Write first 100 bytes to server
    const int bufferSize = 100;
    size_t sizeWritten = writer->pipe_write(&(testString[0]), 1, bufferSize);
    REQUIRE(sizeWritten == bufferSize);

    //Write next 100 bytes to server
    sizeWritten = writer->pipe_write(&(testString[bufferSize]), 1, bufferSize);
    REQUIRE(sizeWritten == bufferSize);

    //Write last 20 bytes to server
    sizeWritten = writer->pipe_write(&(testString[2*bufferSize]), 1, 20);
    REQUIRE(sizeWritten == 20);
    writer->pipe_fflush();
    socket->shutdownRdWr();

}

/**
 * Test export with a normal socket.
 */
TEST_CASE( "ExportHttp", "[http]" ) {
    testExport(test_utils::createSocket());
}

/**
 * Test export with a secure socket.
 */
TEST_CASE( "ExportHttps", "[http]" ) {
    testExport(test_utils::createSecureSocket());
}

/**
 * Purpose of this test is to transfer several chunks between server and client and
 * check if there is no memory issue (buffer overflow etc.) when working with data
 * not fitting into one chunk. (Tests should run with asan enabled).
 * We don't compare the content of the data here.
 */

void testImportBig(std::shared_ptr<exa::Socket> socket) {
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);
    int charReceived = 0;
    //Read from remote until we receive a -1
    do {
        charReceived = reader->fgetc();
    } while(charReceived >= 0);

    socket->shutdownRdWr();
}

/**
 * Test import of huge data with normal socket.
 */
TEST_CASE( "ImportBigHttp", "[http]" ) {
    testImportBig(test_utils::createSocket());
}

/**
 * Test import of huge data with secure socket.
 */
TEST_CASE( "ImportBigHttps", "[http]" ) {
    testImportBig(test_utils::createSecureSocket());
}
