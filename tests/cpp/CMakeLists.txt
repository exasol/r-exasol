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
        ../../src/r-exasol/connection/protocol/http/reader/HttpChunkReader.cpp
        ../../src/r-exasol/connection/protocol/http/reader/HttpChunkReader.h
        ../../src/r-exasol/connection/protocol/http/writer/HttpChunkWriter.cpp
        ../../src/r-exasol/connection/protocol/http/writer/HttpChunkWriter.h
        ../../src/r-exasol/connection/protocol/http/common.h
        ../../src/r-exasol/connection/protocol/http/common.cpp
        ../../src/r-exasol/connection/protocol/http/Chunk.h
        ../../src/r-exasol/connection/protocol/metaInfoReader.h
        ../../src/r-exasol/connection/protocol/metaInfoReader.cpp
        ../../src/r-exasol/connection/ConnectionController.cpp
        ../../src/r-exasol/connection/ConnectionFactoryImpl.h
        ../../src/r-exasol/connection/ConnectionFactoryImpl.cpp
        ../../src/r-exasol/connection/ConnectionController.h
        ../../src/r-exasol/connection/socket/Socket.h
        ../../src/r-exasol/connection/ErrorHandler.h
        ../../src/r-exasol/connection/OdbcAsyncExecutor.h
        ../../src/r-exasol/connection/OdbcSessionInfo.h
        ../../src/r-exasol/connection/Reader.h
        ../../src/r-exasol/connection/Writer.h
        ../../src/r-exasol/connection/ConnectionFactory.h
        ../../src/r-exasol/connection/socket/SocketImpl.h
        ../../src/r-exasol/connection/socket/SocketImpl.cpp
        #../../src/exasol.c
        main.cpp)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
