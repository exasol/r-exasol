#include "catch2/catch.hpp"

#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/connection_controller.h>
#include <iostream>
#include "test_utils.h"
#include "mocks/CustomAsyncSessionMock.h"


/**
 * In case of reader test, the
 */
TEST_CASE( "ReaderCloseConnection", "[async]" ) {

    exa::ConnectionFactoryImpl factory;
    exa::ConnectionController connectionController(factory, [](const std::string& error) { std::cout << "ERROR:" << error << std::endl;});
    const bool retVal = connectionController.connect(test_utils::host, test_utils::PORT);
    REQUIRE(retVal);
    REQUIRE(connectionController.getHostInfo().first == "Test");
    REQUIRE(connectionController.getHostInfo().second == 4);
    CustomAsyncSessionMock mockSessionImpl(joinCalled);
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
}
