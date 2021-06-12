cmake_minimum_required(VERSION 3.13)

project(RoteMud)

set(CMAKE_CXX_STANDARD 17)

# code
find_package(Boost REQUIRED serialization)
include_directories(${Boost_INCLUDE_DIRS})

find_package(libssh CONFIG REQUIRED)
include_directories(${libssh_INCLUDE_DIRS})

file(GLOB_RECURSE HEADER_FILES src/*.hxx)
file(GLOB_RECURSE SOURCE_FILES src/*.cxx)

add_library(rote_lib OBJECT ${HEADER_FILES} ${SOURCE_FILES})

#executable
add_executable(rote_mud main.cxx)
target_link_libraries(rote_mud rote_lib)
target_link_libraries(rote_mud ssh)

if(UNIX)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(rote_mud Threads::Threads)
target_link_libraries(rote_mud ${CMAKE_DL_LIBS})
set_target_properties(rote_mud PROPERTIES ENABLE_EXPORTS true)
endif()

# tests
file(GLOB TEST_HEADER_FILES ${PROJECT_SOURCE_DIR}/tests/*.hxx)
file(GLOB TEST_SOURCE_FILES ${PROJECT_SOURCE_DIR}/tests/*.cxx)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(rote_tests ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES})
target_link_libraries(rote_tests ${Boost_LIBRARIES})
target_link_libraries(rote_tests rote_lib)
target_link_libraries(rote_tests ssh)

enable_testing()
add_test(RoteTests rote_tests)