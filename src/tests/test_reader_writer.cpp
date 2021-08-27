/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

#ifdef WITH_UNIT_TESTS
// All test files should include the <testthat.h>
// header file.
#include <testthat.h>

#include <r_exasol/connection/socket/socket.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer/http_chunk_writer.h>
#include <cstring>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

class TestSocket : public exa::Socket {
public:
    virtual void connect(const char * host, uint16_t port) {}

    virtual size_t recv(void *buf, size_t len) {
        size_t retVal = len;
        if (len <= buffer.size()) {
            ::memcpy(buf, buffer.data(), len);
            buffer.erase(buffer.begin(), buffer.begin() + len);
        }
        else if (!buffer.empty()) {
            retVal = buffer.size();
            ::memcpy(buf, buffer.data(), buffer.size());
            buffer.clear();
        } else {
            retVal = -1;
        }
        return retVal;
    }

    ssize_t send(const std::string &buffer) {
        return send(buffer.data(), buffer.size());
    }

    virtual ssize_t send(const void *buf, size_t len) {
        buffer.reserve(buffer.size() + len);
        buffer.insert(buffer.end(), static_cast<const char*>(buf), static_cast<const char*>(buf) + len);
        return len;
    }

    virtual void shutdownWr() {}
    virtual void shutdownRdWr() {}
private:
    std::vector<char> buffer;
};


std::string createTestString() {
    std::ostringstream os;
    std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
    return os.str();
}


// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Transfer unit tests") {

  test_that("test_reader") {
    auto testSocket = std::make_shared<TestSocket>();
    exa::Chunk chunk{};
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(testSocket, chunk);
    std::vector<char> buffer(100);
    std::string testString = createTestString();
    std::stringstream os;
    os << std::hex << testString.size() << std::endl;
    testSocket->send(os.str());
    os = std::stringstream();
    os << testString << '\0' << '\0';

    testSocket->send(os.str());
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    expect_true(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    expect_true(testString.substr(0, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    expect_true(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    expect_true(testString.substr(100, 100) == strRep);
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    expect_true(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    expect_true(testString.substr(200, 20) == strRep);

    os = std::stringstream();
    os << std::hex << 0 << std::endl;
    testSocket->send(os.str());

    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    expect_true(sizeReceived == 0);
  }

  test_that("test_writer") {
    auto testSocket = std::make_shared<TestSocket>();
    exa::Chunk chunk{};

    std::unique_ptr<exa::writer::HttpChunkWriter> writer = std::make_unique<exa::writer::HttpChunkWriter>(testSocket, chunk);
    std::string testString = createTestString();
    const size_t bufferSize = 100;
    size_t sizeWritten = writer->pipe_write(&(testString[0]), 1, bufferSize);
    expect_true(sizeWritten == bufferSize);
    sizeWritten = writer->pipe_write(&(testString[bufferSize]), 1, bufferSize);
    expect_true(sizeWritten == bufferSize);
    sizeWritten = writer->pipe_write(&(testString[2*bufferSize]), 1, 20);
    expect_true(sizeWritten == 20);
    writer->pipe_fflush();

    const std::string header = "HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nContent-type: application/octet-stream\r\nContent-disposition: attachment; filename=data.csv\r\nConnection: close\r\n\r\n";
    std::vector<char> buffer(header.size());
    const size_t recvHeaderSize = testSocket->recv(buffer.data(), header.size());
    expect_true(recvHeaderSize == header.size());
    expect_true(::memcmp(buffer.data(), header.data(), header.size()) == 0);

    buffer.resize(testString.size());
    const size_t recvDataSize = testSocket->recv(buffer.data(), testString.size());

    expect_true(recvDataSize == testString.size());
    expect_true(::memcmp(buffer.data(), testString.data(), testString.size()) == 0);
  }

}

#endif
