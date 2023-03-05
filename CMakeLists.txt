# CMake configuration file of parser project

cmake_minimum_required(VERSION 3.0)

# set language std
set(CMAKE_CXX_STANDARD 20)
# set language std and Disable fallback to a previous version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable CXX Syntax EXTENSIONS
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(WebServer VERSION 1.0)

include_directories("src")

set(BASIC_CXXFLAGS -g -pipe -fPIC -Wall -Wextra -Werror -pedantic-errors -pthread)
set(EXTRA_WARNINGS -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wswitch-default -Wfloat-equal -Wshadow -Wcast-qual -Wextra-semi)
set(DEFAULT_CXXFLAGS ${BASIC_CXXFLAGS} ${EXTRA_WARNINGS})
set(CXXFLAGS_PROD ${DEFAULT_CXXFLAGS} -O2)
# add_compile_options(${DEFAULT_CXXFLAGS})


file(GLOB COMMON_SRC src/common/*.cc)
file(GLOB UTILS_SRC src/utils/*.cc)

file(GLOB SERVER_SRC src/server/*.cc)
file(GLOB CLIENT_SRC src/client/*.cc)

file(GLOB SERVER_MAIN_SRC src/main.cc)


set(SERVER_BIN_NAME server)
add_executable(${SERVER_BIN_NAME} ${SERVER_MAIN_SRC} ${SERVER_SRC} ${COMMON_SRC} ${UTILS_SRC})
target_compile_options(
    ${SERVER_BIN_NAME}
    PRIVATE
    ${CXXFLAGS_PROD}
)
target_link_libraries(${SERVER_BIN_NAME} pthread)
set_target_properties(
    ${SERVER_BIN_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set(CLIENT_BIN_NAME client)
add_executable(${CLIENT_BIN_NAME} ${CLIENT_SRC} ${COMMON_SRC} ${UTILS_SRC})
target_compile_options(
    ${CLIENT_BIN_NAME}
    PRIVATE
    ${CXXFLAGS_PROD}
)
target_link_libraries(${CLIENT_BIN_NAME} pthread)
set_target_properties(
    ${CLIENT_BIN_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set(THREAD_POOL_TEST_BIN_NAME thread_pool_test)
file(GLOB THREAD_POOL_SRC tests/ThreadPoolTest.cc)
add_executable(${THREAD_POOL_TEST_BIN_NAME} ${THREAD_POOL_SRC} ${SERVER_SRC} ${COMMON_SRC} ${UTILS_SRC})
target_compile_options(
    ${THREAD_POOL_TEST_BIN_NAME}
    PRIVATE
    ${CXXFLAGS_PROD}
)
target_link_libraries(${THREAD_POOL_TEST_BIN_NAME} pthread)
set_target_properties(
    ${THREAD_POOL_TEST_BIN_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)
