#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include <r_exasol/connection//protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer//http_chunk_writer.h>
#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/connection_controller.h>
#include <sstream>

#include "mocks/AsyncSessionMock.h"
#include "test_utils.h"


/**
 * Because of asynchronous behavior, we wait one second before starting test, to give server (Python program)
 * time to prepare server socket.
 */
struct MyListener : Catch::TestEventListenerBase {

    using TestEventListenerBase::TestEventListenerBase; // inherit constructor

    void testCaseStarting(Catch::TestCaseInfo const&) override {
#ifdef _WIN32
        _sleep(1);
#else
        ::sleep(1);
#endif
    }

    void testCaseEnded(Catch::TestCaseStats const&) override {
        // Tear-down after a test case is run
    }
};

CATCH_REGISTER_LISTENER( MyListener )


