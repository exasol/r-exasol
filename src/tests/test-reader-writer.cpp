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

#include <impl/socket/Socket.h>
#include <impl/transfer/import/HttpChunkReader.h>
#include <cstring>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <sstream>

class TestSocket : public exa::Socket {
public:
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
        send(buffer.data(), buffer.size());
    }

    virtual ssize_t send(const void *buf, size_t len) {
        buffer.reserve(buffer.size() + len);
        buffer.insert(buffer.end(), static_cast<const char*>(buf), static_cast<const char*>(buf) + len);

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

  // The format for specifying tests is similar to that of
  // testthat's R functions. Use 'test_that()' to define a
  // unit test, and use 'expect_true()' and 'expect_false()'
  // to test the desired conditions.
  test_that("test_reader") {
    TestSocket testSocket;
    exa::Chunk chunk{};
    std::unique_ptr<exa::import::HttpChunkReader> reader = std::make_unique<exa::import::HttpChunkReader>(testSocket, chunk);
    std::vector<char> buffer(100);
    std::string testString = createTestString();
    std::stringstream os;
    os << std::hex << testString.size() << std::endl;
    testSocket.send(os.str());
    os = std::stringstream();
    os << testString << '\0' << '\0';

    testSocket.send(os.str());
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    expect_true(sizeReceived == buffer.size());
    std::string strRep(buffer.data());
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
    testSocket.send(os.str());

    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    expect_true(sizeReceived == 0);
  }

}

#endif