cmake_minimum_required(VERSION 3.19)
project(r_exasol)

set(CMAKE_CXX_STANDARD 14)
include_directories(../../src /usr/share/R/include/)

include(FetchContent)
include(CTest)

FetchContent_Declare(
        catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.6)

# CMake 3.14+
FetchContent_MakeAvailable(catch)

add_executable(r_exasol
        ../../src/connection/protocol/http/reader/HttpChunkReader.cpp
        ../../src/connection/protocol/http/reader/HttpChunkReader.h
        ../../src/connection/protocol/http/writer/HttpChunkWriter.cpp
        ../../src/connection/protocol/http/writer/HttpChunkWriter.h
        ../../src/connection/protocol/http/common.h
        ../../src/connection/protocol/http/common.cpp
        ../../src/connection/protocol/http/Chunk.h
        ../../src/connection/protocol/metaInfoReader.h
        ../../src/connection/protocol/metaInfoReader.cpp
        ../../src/odbc/OdbcAsyncExecutorImpl.h
        ../../src/odbc/OdbcAsyncExecutorImpl.cpp
        ../../src/connection/ConnectionController.cpp
        ../../src/if/ConnectionController.h
        ../../src/if/Socket.h
        ../../src/if/ErrorHandler.h
        ../../src/if/ExaTypes.h
        ../../src/if/OdbcAsyncExecutor.h
        ../../src/if/OdbcSessionInfo.h
        ../../src/if/Reader.h
        ../../src/if/Writer.h
        ../../src/if/ConnectionFactory.h
        ../../src/socket/SocketImpl.h
        ../../src/socket/SocketImpl.cpp
        ../../src/exasol.c
        main.cpp)

target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
