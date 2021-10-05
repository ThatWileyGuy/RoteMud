cmake_minimum_required(VERSION 3.13)

project(RoteMud)

set(CMAKE_CXX_STANDARD 20)

# code
find_package(Boost REQUIRED serialization unit_test_framework)
find_package(libssh CONFIG REQUIRED)

file(GLOB_RECURSE HEADER_FILES src/*.hxx)
file(GLOB_RECURSE SOURCE_FILES src/*.cxx)

add_library(rote_lib OBJECT ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(rote_lib PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rote_lib PRIVATE ${libssh_INCLUDE_DIRS})

#executable
add_executable(rote_mud main.cxx)
target_link_libraries(rote_mud rote_lib)
target_link_libraries(rote_mud ssh)


if(UNIX)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(rote_mud Threads::Threads)
target_link_libraries(rote_mud ${CMAKE_DL_LIBS})
set_target_properties(rote_mud ENABLE_EXPORTS true)
endif()

# tests
file(GLOB TEST_HEADER_FILES ${PROJECT_SOURCE_DIR}/tests/*.hxx)
file(GLOB TEST_SOURCE_FILES ${PROJECT_SOURCE_DIR}/tests/*.cxx)

add_executable(rote_tests ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES})
target_compile_definitions(rote_tests PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(rote_tests Boost::boost Boost::serialization Boost::unit_test_framework)
target_link_libraries(rote_tests rote_lib)
target_link_libraries(rote_tests ssh)
target_include_directories(rote_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rote_tests PRIVATE src)

enable_testing()
add_test(RoteTests rote_tests)

