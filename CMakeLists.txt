# CMake configuration file of parser project

cmake_minimum_required(VERSION 3.10)

# set language std
set(CMAKE_CXX_STANDARD 20)
# set language std and Disable fallback to a previous version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable CXX Syntax EXTENSIONS
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(WebServer
        VERSION 0.1
        DESCRIPTION "WebServer"
        LANGUAGES CXX
        )

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

######################################################################################################################
# DEPENDENCIES BEGIN
######################################################################################################################

# Expected directory structure.
set(BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")

# clang-format
if (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_FORMAT_BIN
            NAMES clang-format clang-format-8
            HINTS ${CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-format.")
else ()
    message(STATUS "Found clang-format at ${CLANG_FORMAT_BIN}")
endif ()

# clang-tidy
if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-fidy-8
            HINTS ${CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-tidy.")
else ()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "Found clang-fidy at ${CLANG_TIDY_BIN}")
endif ()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${BUILD_SUPPORT_DIR})
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "Couldn't find cpplint.")
else ()
    message(STATUS "Found cpplint at ${CPPLINT_BIN}")
endif ()

######################################################################################################################
# DEPENDENCIES END
######################################################################################################################

######################################################################################################################
# COMPILER SETUP
######################################################################################################################

include_directories("src/include")

# OS
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Platform: macOS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_MACOS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Platform: Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_LINUX")
endif()

set(BASIC_CXXFLAGS "-O2 -g -pipe -fPIC -Wall -Wextra -Werror -pedantic-errors -pthread")
set(EXTRA_WARNINGS "-Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wswitch-default -Wfloat-equal -Wshadow -Wcast-qual -Wextra-semi -Wno-unused-parameter -Wno-attributes")
# set(DEFAULT_CXXFLAGS ${BASIC_CXXFLAGS} ${EXTRA_WARNINGS})
# add_compile_options(${DEFAULT_CXXFLAGS})

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BASIC_CXXFLAGS} ${EXTRA_WARNINGS}")
# cmake -DCMAKE_BUILD_TYPE=DEBUG ..
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

set(GCC_COVERAGE_LINK_FLAGS "-fPIC")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)
add_subdirectory(test)

######################################################################################################################
# COMPILER END
######################################################################################################################

######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make format"
# "make check-format"
##########################################
string(CONCAT FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
        )

# runs clang format and updates files in place.
add_custom_target(format ${BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FORMAT_DIRS}
        --fix
        --quiet
        )
    
# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FORMAT_DIRS}
        --quiet
        )

##########################################
# "make cpplint"
##########################################

file(GLOB_RECURSE LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc"
        )

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(cpplint echo '${LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting,-whitespace/indent,-readability/todo,-readability/nolint,-build/c++11,-build/include_what_you_use,-runtime/int,-runtime/references
        )

###########################################################
# "make clang-tidy" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(clang-tidy
        ${BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )
# add_dependencies(check-clang-tidy shared)                    # needs gtest headers, compile_commands.json
