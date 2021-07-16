cmake_minimum_required(VERSION 3.19)
project(r_exasol)

set(CMAKE_CXX_STANDARD 14)
include_directories(../../src/core /usr/share/R/include/)

include(FetchContent)
include(CTest)

FetchContent_Declare(
        catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.6)

# CMake 3.14+
FetchContent_MakeAvailable(catch)

add_executable(r_exasol
        ../../src/core/impl/protocol/http/reader/HttpChunkReader.cpp
        ../../src/core/impl/protocol/http/reader/HttpChunkReader.h
        ../../src/core/impl/protocol/http/writer/HttpChunkWriter.cpp
        ../../src/core/impl/protocol/http/writer/HttpChunkWriter.h
        ../../src/core/impl/protocol/http/common.h
        ../../src/core/impl/protocol/http/common.cpp
        ../../src/core/impl/protocol/http/Chunk.h
        ../../src/core/impl/protocol/metaInfoReader.h
        ../../src/core/impl/protocol/metaInfoReader.cpp
        ../../src/odbc/OdbcAsyncExecutorImpl.h
        ../../src/odbc/OdbcAsyncExecutorImpl.cpp
        ../../src/core/impl/ConnectionController.cpp
        ../../src/core/if/ConnectionController.h
        ../../src/core/if/Socket.h
        ../../src/core/if/ErrorHandler.h
        ../../src/core/if/ExaTypes.h
        ../../src/core/if/OdbcAsyncExecutor.h
        ../../src/core/if/OdbcSessionInfo.h
        ../../src/core/if/Reader.h
        ../../src/core/if/Writer.h
        ../../src/core/if/ConnectionFactory.h
        ../../src/core/impl/socket/SocketImpl.h
        ../../src/core/impl/socket/SocketImpl.cpp
        ../../src/exasol.c
        main.cpp)

target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
