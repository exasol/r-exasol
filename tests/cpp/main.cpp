//
// Created by thomas on 09/07/2021.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include <impl/transfer/import/HttpChunkReader.h>
#include <impl/transfer/export//HttpChunkWriter.h>
#include <impl/socket/SocketImpl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <arpa/inet.h>


const int PORT = 5000;

std::string createTestString() {
    std::ostringstream os;
    std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
    return os.str();
}

tSocket openSocket() {
    const char *host = "localhost";
    ::sleep(1);
    struct sockaddr_in serv_addr;
    struct hostent *server;

    tSocket  s = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE (s > 0);

    ::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    server = gethostbyname(host);
    REQUIRE (server != NULL);
    ::memcpy((char *)&serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    REQUIRE (::connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0);

    return s;
}


TEST_CASE( "Import", "[import]" ) {

    std::unique_ptr<exa::Socket> socket = std::make_unique<exa::SocketImpl>(openSocket());
    exa::Chunk chunk{};
    std::unique_ptr<exa::import::HttpChunkReader> reader = std::make_unique<exa::import::HttpChunkReader>(*socket, chunk);
    std::vector<char> buffer(100);
    std::string testString = createTestString();
    size_t sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    std::string strRep(buffer.data());
    REQUIRE(testString.substr(0, 100) == strRep);
    std::cerr << "First buffer" << std::endl;
    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == buffer.size());
    strRep = std::string(buffer.data(), buffer.size());
    REQUIRE(testString.substr(100, 100) == strRep);
    std::cerr << "Second buffer" << std::endl;
    sizeReceived = reader->pipe_read(buffer.data(), 1, 20);
    REQUIRE(sizeReceived == 20);
    std::cerr << "Last buffer" << std::endl;
    strRep = std::string(buffer.data(), 20);
    REQUIRE(testString.substr(200, 20) == strRep);

    sizeReceived = reader->pipe_read(buffer.data(), 1, buffer.size());
    REQUIRE(sizeReceived == 0);

}


TEST_CASE( "Export", "[export]" ) {
}
