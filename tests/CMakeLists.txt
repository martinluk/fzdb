project(fuzzy-database-tests)
cmake_minimum_required(VERSION 2.8.7)
cmake_policy(VERSION 2.8.7)

message("Building Tests")

include(${CMAKE_CURRENT_SOURCE_DIR}/../build_modules/pthreads.cmake)

FILE(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp)
FILE(GLOB TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${GTEST_INCLUDE_DIRS}) 
include_directories(${GMOCK_INCLUDE_DIRS}) 

add_executable(fuzzy-database-test ${TEST_SOURCES} ${TEST_HEADERS} $<TARGET_OBJECTS:gtest> $<TARGET_OBJECTS:gmock> $<TARGET_OBJECTS:fuzzy-database-lib>)
add_dependencies(fuzzy-database-test nowide)
add_dependencies(fuzzy-database-test gtest)
add_dependencies(fuzzy-database-test fuzzy-database-lib)

target_link_libraries(fuzzy-database-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fuzzy-database-test ${Boost_LIBRARIES})
target_link_libraries(fuzzy-database-test ${Boost_Nowide_LIBRARIES})

add_test(NAME test COMMAND fuzzy-database-test)
