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
        ../../src/r-exasol/connection/protocol/http/reader/http_chunk_reader.cpp
        ../../src/r-exasol/connection/protocol/http/reader/http_chunk_reader.h
        ../../src/r-exasol/connection/protocol/http/writer/http_chunk_writer.cpp
        ../../src/r-exasol/connection/protocol/http/writer/http_chunk_writer.h
        ../../src/r-exasol/connection/protocol/http/common.h
        ../../src/r-exasol/connection/protocol/http/common.cpp
        ../../src/r-exasol/connection/protocol/http/chunk.h
        ../../src/r-exasol/connection/protocol/meta_info_reader.h
        ../../src/r-exasol/connection/protocol/meta_info_reader.cpp
        ../../src/r-exasol/connection/connection_controller.cpp
        ../../src/r-exasol/connection/connection_factory_impl.h
        ../../src/r-exasol/connection/connection_factory_impl.cpp
        ../../src/r-exasol/connection/connection_controller.h
        ../../src/r-exasol/connection/socket/socket.h
        ../../src/r-exasol/connection/error_handler.h
        ../../src/r-exasol/connection/odbc_async_executor.h
        ../../src/r-exasol/connection/odbc_session_info.h
        ../../src/r-exasol/connection/reader.h
        ../../src/r-exasol/connection/writer.h
        ../../src/r-exasol/connection/connection_factory.h
        ../../src/r-exasol/connection/socket/socket_impl.h
        ../../src/r-exasol/connection/socket/socket_impl.cpp
        #../../src/exasol.c
        main.cpp)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
