cmake_minimum_required(VERSION 3.19)
project(r_exasol)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../src/core /usr/share/R/include)



include(FetchContent)
include(CTest)

FetchContent_Declare(
        catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.6)

# CMake 3.14+
FetchContent_MakeAvailable(catch)

add_executable(r_exasol
        ../../src/core/impl/transfer/import/HttpChunkReader.cpp
        ../../src/core/impl/transfer/import/HttpChunkReader.h
        ../../src/core/impl/transfer/export/HttpChunkWriter.cpp
        ../../src/core/impl/transfer/export/HttpChunkWriter.h
        ../../src/core/impl/rconnection/RReaderConnection.cpp
        ../../src/core/impl/rconnection/RReaderConnection.h
        ../../src/core/impl/rconnection/RWriterConnection.cpp
        ../../src/core/impl/rconnection/RWriterConnection.h
        ../../src/core/impl/transfer/chunk.h
        ../../src/core/impl/ConnectionContext.h
        ../../src/core/connection_gen.cpp
        ../../src/core/connection_gen.h
        ../../src/core/impl/socket/Socket.h
        ../../src/core/impl/socket/SocketImpl.h
        ../../src/core/impl/socket/SocketImpl.cpp
#        ../../src/exasol.c
        main.cpp)

target_link_libraries(r_exasol PRIVATE Catch2::Catch2 R)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
