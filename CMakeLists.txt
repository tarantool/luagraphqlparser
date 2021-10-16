cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(luagraphqlparser C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

# Find Tarantool and Lua dependencies
set(Tarantool_FIND_REQUIRED ON)
find_package(Tarantool)
include_directories(${TARANTOOL_INCLUDE_DIRS})

# Find other dependencies

add_subdirectory(libgraphqlparser EXCLUDE_FROM_ALL)
include_directories("${CMAKE_SOURCE_DIR}/libgraphqlparser" "${CMAKE_BINARY_DIR}/libgraphqlparser")

# Set CFLAGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -std=c11")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
# Set CXXFLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=gnu++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")

# Build module
add_subdirectory(luagraphqlparser)
