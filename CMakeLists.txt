cmake_minimum_required(VERSION 3.26)

project(RoteMud)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# libraries
find_package(Boost REQUIRED serialization unit_test_framework)
find_package(libssh CONFIG REQUIRED)

# core
file(GLOB_RECURSE CORE_HEADER_FILES core/*.hxx)
file(GLOB_RECURSE CORE_SOURCE_FILES core/*.cxx)
file(GLOB_RECURSE CORE_MODULE_FILES core/*.cppm)

add_library(rote_core OBJECT ${CORE_HEADER_FILES} ${CORE_SOURCE_FILES})
target_include_directories(rote_core PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rote_core PRIVATE ${libssh_INCLUDE_DIRS})
target_sources(rote_core
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${CORE_MODULE_FILES}
)


# executable
add_executable(rote_mud main.cxx)
target_link_libraries(rote_mud rote_core)
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

