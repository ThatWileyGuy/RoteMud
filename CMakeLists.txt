cmake_minimum_required(VERSION 3.13)

project(RoteMud)

set(CMAKE_CXX_STANDARD 20)

# libraries
find_package(Boost REQUIRED serialization unit_test_framework)
find_package(libssh CONFIG REQUIRED)

# core
file(GLOB_RECURSE CORE_HEADER_FILES core/*.hxx)
file(GLOB_RECURSE CORE_SOURCE_FILES core/*.cxx)

add_library(rote_core OBJECT ${CORE_HEADER_FILES} ${CORE_SOURCE_FILES})
target_include_directories(rote_core PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rote_core PRIVATE ${libssh_INCLUDE_DIRS})
target_precompile_headers(rote_core PRIVATE core/mud.hxx)

# db
file(GLOB_RECURSE DB_HEADER_FILES db/*.hxx)
file(GLOB_RECURSE DB_SOURCE_FILES db/*.cxx)

add_library(rote_db OBJECT ${DB_HEADER_FILES} ${DB_SOURCE_FILES})
target_include_directories(rote_db PRIVATE ${Boost_INCLUDE_DIRS})

# executable
add_executable(rote_mud main.cxx)
target_link_libraries(rote_mud rote_core)
target_link_libraries(rote_mud rote_db)
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

add_executable(rote_tests ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES})
target_compile_definitions(rote_tests PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(rote_tests Boost::boost Boost::serialization Boost::unit_test_framework)
target_link_libraries(rote_tests rote_core)
target_link_libraries(rote_tests ssh)
target_include_directories(rote_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rote_tests PRIVATE core)

if(UNIX)
target_link_libraries(rote_tests Threads::Threads)
target_link_libraries(rote_tests ${CMAKE_DL_LIBS})
endif()

enable_testing()
add_test(RoteTests rote_tests)

