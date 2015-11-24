project(fuzzy-database-tests)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

message("Building Tests")

include(${CMAKE_CURRENT_SOURCE_DIR}/../build_modules/pthreads.cmake)

FILE(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp)
FILE(GLOB TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${GTEST_INCLUDE_DIRS}) 

add_executable(fuzzy-database-test ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(fuzzy-database-test ${GTEST_LIBRARIES})
target_link_libraries(fuzzy-database-test ${CMAKE_THREAD_LIBS_INIT})
