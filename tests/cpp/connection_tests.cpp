#include "catch2/catch.hpp"

#include <r_exasol/connection/socket/socket_impl.h>

#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/connection_controller.h>
#include <iostream>
#include "test_utils.h"
#include "mocks/AsyncSessionMock.h"


/**
 * Purpose of this test is to verify behavior of ConnectionController:
 * 1. Test correct reading of the metadata (remote host + port)
 * 2. Creation of reader object
 * 3. Testing reading of the HttpHeader
 * 4. Testing correct reading of the Http payload (chunk), by comparing content (@see test_utils::createTestString())
 * 5. Correct behavior of the asynchronous executor during shutdown (calling join on std::thread()).
 */
void testConnectionControllerImport(exa::ProtocolType protocolType) {
    bool errorCalled = false;
    bool joinCalled = false;
    exa::ConnectionFactoryImpl factory;
    //Instantiate controller and declare error callback as a lambda.
    exa::ConnectionController connectionController(factory, [&errorCalled](const std::string& error) {
        std::cout << "ERROR:" << error << std::endl;
        errorCalled = true;
    });

    //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
    const bool retVal = connectionController.connect(protocolType, test_utils::host,
                                                     test_utils::PORT, test_utils::getCertificate());
    REQUIRE(retVal);
    REQUIRE(connectionController.getProxyHost() == "Test");
    REQUIRE(connectionController.getProxyPort() == 4);

    //Create our async mock which uses the std::thread implementation.
    //This implementation does nothing in the background and returns immediately.
    //When join is called, it sets the given parameter (@param joinCalled).
    AsyncSessionMock mockSessionImpl(joinCalled);

    //Create reader
    std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl);
    REQUIRE(!readerWeak.expired());
    auto reader = readerWeak.lock();

    //Create test data
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString(); //returns 220 bytes of test data

    //Read first 100 bytes of data from remote (Python program) and compare content
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);

    //Read next 100 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);

    //Read last 20 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);

    //Read terminating 0
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);

    //Shutdown connection controller.
    connectionController.shutDown();

    //No error is supposed to happen in this test
    REQUIRE(!errorCalled);

}

TEST_CASE( "ConnectionControllerImportHttp", "[connection]" ) {
    testConnectionControllerImport(exa::ProtocolType::http);
}

TEST_CASE( "ConnectionControllerImportHttps", "[connection]" ) {
    testConnectionControllerImport(exa::ProtocolType::https);
}

/**
 * Purpose of this test is to verify if we can successful read data after writing to server,
 * with other words we test if no garbage remains in memory after one cycle, and that we can instatiate a clean
 * reaer object after we used a writer object.
 *
 */
void testConnectionControllerEcho(exa::ProtocolType protocolType) {
    exa::ConnectionFactoryImpl factory;
    SECTION( "first testing writing to server" )
    {
        bool errorCalled = false;
        //Instantiate controller and declare error callback as a lambda.
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });
        bool joinCalled(false);
        //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
        const bool retVal = connectionController.connect(protocolType, test_utils::host,
                                                         test_utils::PORT, test_utils::getCertificate());
        REQUIRE(retVal);
        REQUIRE(connectionController.getProxyHost() == "Test");
        REQUIRE(connectionController.getProxyPort() == 4);

        //Create our async mock which uses the std::thread implementation.
        //This implementation does nothing in the background and returns immediately.
        //When join is called, it sets the given parameter (@param joinCalled).
        AsyncSessionMock mockSessionImpl(joinCalled);

        //create writer
        std::weak_ptr<exa::writer::Writer> writer_weak = connectionController.startWriting(mockSessionImpl);
        REQUIRE(!writer_weak.expired());
        auto writer = writer_weak.lock();

        //Write 2 lines of test data:
        // "a"
        // "b"
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

        //shutdown controller
        connectionController.shutDown();

        //shutdown controller must have called join to the background thread
        REQUIRE(joinCalled);

        //No error is supposed to happen in this test
        REQUIRE(!errorCalled);
    }
    SECTION( "now testing reading from server" )
    {
        bool errorCalled = false;
        bool joinCalled(false);
        //Instantiate controller and declare error callback as a lambda.
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });

        //Connection to remote (Python program) and read meta data (hostname = Test, port number = 4)
        const bool retVal = connectionController.connect(protocolType, test_utils::host, test_utils::PORT, test_utils::getCertificate());
        REQUIRE(retVal);
        REQUIRE(connectionController.getProxyHost() == "Test");
        REQUIRE(connectionController.getProxyPort() == 4);

        //Create our async mock which uses the std::thread implementation.
        //This implementation does nothing in the background and returns immediately.
        //When join is called, it sets the given parameter (@param joinCalled).
        AsyncSessionMock mockSessionImpl(joinCalled);
        std::weak_ptr<exa::reader::Reader> readerWeak = connectionController.startReading(mockSessionImpl);
        REQUIRE(!readerWeak.expired());
        auto reader = readerWeak.lock();

        //Read back the data from the server (python program) which have been sent in the first step
        std::stringstream data;
        do {
            const int c = reader->fgetc();
            if (-1 == c) break;
            data << static_cast<char>(c);
        } while (true);

        //Compare imported content with oiginal sent content
        REQUIRE(std::string("Name\na\nb") == data.str());

        //Shutdown controller
        connectionController.shutDown();

        //Check that connection controller joined the background thread.
        REQUIRE(joinCalled);

        //No error is supposed to happen in this test.
        REQUIRE(!errorCalled);
    }
}

TEST_CASE( "ConnectionControllerEchoHttp", "[connection]" ) {
    testConnectionControllerEcho(exa::ProtocolType::http);
}

TEST_CASE( "ConnectionControllerEchoHttps", "[connection]" ) {
    testConnectionControllerEcho(exa::ProtocolType::https);
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
        //Instantiate controller and declare error callback as a lambda.
        exa::ConnectionController connectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });

        //Connection to remote (Python program) and try to read meta data (hostname = Test, port number = 4)
        const bool retVal = connectionController.connect(exa::ProtocolType::http, test_utils::host,
                                                         test_utils::PORT, test_utils::getCertificate());
        //Server aborted connection before sending meta data.
        REQUIRE(!retVal);

        //Error callback must have been called.
        REQUIRE(errorCalled);

        //Shutdown connection controller
        connectionController.shutDown();
    }
    SECTION( "now testing success case" )
    {
        bool errorCalled = false;
        //Instantiate controller and declare error callback as a lambda.
        exa::ConnectionController newConnectionController(factory, [&errorCalled](const std::string &error) {
            std::cout << "ERROR:" << error << std::endl;
            errorCalled = true;
        });

        //Connection to remote (Python program) and try to read meta data (hostname = Test, port number = 4)
        const bool retVal = newConnectionController.connect(exa::ProtocolType::http, test_utils::host,
                                                            test_utils::PORT, test_utils::getCertificate());
        bool joinCalled(false);
        REQUIRE(retVal);
        REQUIRE(newConnectionController.getProxyHost() == "Test");
        REQUIRE(newConnectionController.getProxyPort() == 4);


        //Create our async mock which uses the std::thread implementation.
        //This implementation does nothing in the background and returns immediately.
        //When join is called, it sets the given parameter (@param joinCalled).
        AsyncSessionMock mockSessionImpl(joinCalled);

        //Create reader
        std::weak_ptr<exa::reader::Reader> readerWeak = newConnectionController.startReading(mockSessionImpl);
        REQUIRE(!readerWeak.expired());
        auto reader = readerWeak.lock();

        //Create test data
        std::vector<char> buffer(100);
        std::string testString = test_utils::createTestString(); //returns 220 bytes of test data

        //Read first 100 bytes of data from remote (Python program) and compare content
        size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
        REQUIRE(sizeReceived == buffer.size());
        std::string strRep(buffer.data(), buffer.size());
        REQUIRE(testString.substr(0, 100) == strRep);

        //Read next 100 bytes and compare content
        sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
        REQUIRE(sizeReceived == buffer.size());
        strRep = std::string(buffer.data(), buffer.size());
        REQUIRE(testString.substr(100, 100) == strRep);

        //Read last 20 bytes and compare content
        sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
        REQUIRE(sizeReceived == 20);
        strRep = std::string(buffer.data(), 20);
        REQUIRE(testString.substr(200, 20) == strRep);

        //Read terminating 0
        sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
        REQUIRE(sizeReceived == 0);

        //Shutdown connection controller
        newConnectionController.shutDown();

        //Assume that connection controller joined background thread.
        REQUIRE(joinCalled);

        //No error is supposed to happen in this part of the test
        REQUIRE(!errorCalled);
    }
}

/**
 * Purpose of this test is to verify behavior of HttpChunkReader:
 * 1. Testing reading of the HttpHeader
 * 2. Testing correct reading of the Http payload (chunk), by comparing content (@see test_utils::createTestString())
 */
void testHttpProtocol(std::shared_ptr<exa::Socket> socket) {
    exa::Chunk chunk{};

    //Create reader instance
    std::unique_ptr<exa::reader::HttpChunkReader> reader = std::make_unique<exa::reader::HttpChunkReader>(socket, chunk);

    //This will read the Http Header.
    reader->start();

    //Create test data
    std::vector<char> buffer(100);
    std::string testString = test_utils::createTestString(); //returns 220 bytes of test data

    //Read first 100 bytes of data from remote (Python program) and compare content
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data(), buffer.size());
    REQUIRE(testString.substr(0, 100) == strRep);

    //Read next 100 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);

    //Read last 20 bytes and compare content
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);

    //Read terminating 0
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);
    socket->shutdownRdWr();
}

TEST_CASE( "ProtocolHttp", "[connection]" ) {
    testHttpProtocol(test_utils::createSocket());
}


TEST_CASE( "ProtocolHttps", "[connection]" ) {
    testHttpProtocol(test_utils::createSecureSocket());
}
