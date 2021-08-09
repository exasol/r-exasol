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
        ../../src/r_exasol/connection/protocol/http/reader/http_chunk_reader.cpp
        ../../src/r_exasol/connection/protocol/http/reader/http_chunk_reader.h
        ../../src/r_exasol/connection/protocol/http/writer/http_chunk_writer.cpp
        ../../src/r_exasol/connection/protocol/http/writer/http_chunk_writer.h
        ../../src/r_exasol/connection/protocol/http/common.h
        ../../src/r_exasol/connection/protocol/http/common.cpp
        ../../src/r_exasol/connection/protocol/http/chunk.h
        ../../src/r_exasol/connection/protocol/meta_info_reader.h
        ../../src/r_exasol/connection/protocol/meta_info_reader.cpp
        ../../src/r_exasol/connection/connection_controller.cpp
        ../../src/r_exasol/connection/connection_factory_impl.h
        ../../src/r_exasol/connection/connection_factory_impl.cpp
        ../../src/r_exasol/connection/connection_controller.h
        ../../src/r_exasol/connection/socket/socket.h
        ../../src/r_exasol/connection/error_handler.h
        ../../src/r_exasol/connection/async_executor/async_executor.h
        ../../src/r_exasol/connection/async_executor/async_executor_session_info.h
        ../../src/r_exasol/connection/reader.h
        ../../src/r_exasol/connection/writer.h
        ../../src/r_exasol/connection/connection_factory.h
        ../../src/r_exasol/connection/socket/socket_impl.h
        ../../src/r_exasol/connection/socket/socket_impl.cpp
        #../../src/exasol.c
        main.cpp
        http_tests.cpp
        connection_tests.cpp
        async_tests.cpp
        test_utils.h
        mocks/AsyncExecutorMock.cpp
        mocks/AsyncExecutorMock.h
        mocks/AsyncSessionMock.cpp
        mocks/AsyncSessionMock.h
        mocks/CustomAsyncExecutorMock.cpp
        mocks/CustomAsyncExecutorMock.h
        mocks/CustomAsyncSessionMock.cpp
        mocks/CustomAsyncSessionMock.h)

if (WIN32)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wa,-mbig-obj")
    target_link_libraries(r_exasol PRIVATE Catch2::Catch2 wsock32 ws2_32)
else()
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    target_link_libraries(r_exasol PRIVATE Catch2::Catch2)
endif()

set(TEST_EXEC "python3" "${CMAKE_SOURCE_DIR}/python_server_tests.py")
add_test(NAME r_exasol COMMAND ${TEST_EXEC})
