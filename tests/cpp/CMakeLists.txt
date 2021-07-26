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
        ../../src/r-exasol/impl/connection/protocol/http/reader/HttpChunkReader.cpp
        ../../src/r-exasol/impl/connection/protocol/http/reader/HttpChunkReader.h
        ../../src/r-exasol/impl/connection/protocol/http/writer/HttpChunkWriter.cpp
        ../../src/r-exasol/impl/connection/protocol/http/writer/HttpChunkWriter.h
        ../../src/r-exasol/impl/connection/protocol/http/common.h
        ../../src/r-exasol/impl/connection/protocol/http/common.cpp
        ../../src/r-exasol/impl/connection/protocol/http/Chunk.h
        ../../src/r-exasol/impl/connection/protocol/metaInfoReader.h
        ../../src/r-exasol/impl/connection/protocol/metaInfoReader.cpp
        ../../src/r-exasol/impl/connection/ConnectionController.cpp
        ../../src/r-exasol/impl/connection/ConnectionFactoryImpl.h
        ../../src/r-exasol/impl/connection/ConnectionFactoryImpl.cpp
        ../../src/r-exasol/impl/connection/ConnectionController.h
        ../../src/r-exasol/impl/connection/socket/Socket.h
        ../../src/r-exasol/if/ErrorHandler.h
        ../../src/r-exasol/if/ExaTypes.h
        ../../src/r-exasol/if/OdbcAsyncExecutor.h
        ../../src/r-exasol/if/OdbcSessionInfo.h
        ../../src/r-exasol/if/Reader.h
        ../../src/r-exasol/if/Writer.h
        ../../src/r-exasol/impl/connection/ConnectionFactory.h
        ../../src/r-exasol/impl/connection/socket/SocketImpl.h
        ../../src/r-exasol/impl/connection/socket/SocketImpl.cpp
        #../../src/exasol.c
        main.cpp)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
