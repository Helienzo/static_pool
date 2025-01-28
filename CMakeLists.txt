cmake_minimum_required(VERSION 3.13)

project(static_pool C ASM)

add_library(static_pool INTERFACE)

target_sources(static_pool INTERFACE
	src/static_pool.c
)

target_include_directories(static_pool INTERFACE
	src
)

# Option to build standalone executable for testing
option(STATIC_POOL_TEST "Build standalone executable for static_pool" OFF)

if(STATIC_POOL_TEST)
    set(CMAKE_C_COMPILER gcc)

    # Add standalone executable for testing static_pool
    add_executable(test_static_pool test/test_static_pool.c)

    # Link the static_pool library to the standalone executable
    target_link_libraries(test_static_pool PRIVATE static_pool)

    # Optionally, add any specific compiler options for testing
    target_compile_options(test_static_pool PRIVATE -Wall -Wextra -pedantic)
endif()