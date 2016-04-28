project(fzdb-tests)
cmake_minimum_required(VERSION 2.8.7)
cmake_policy(VERSION 2.8.7)

message("Building Tests")

include(${CMAKE_CURRENT_SOURCE_DIR}/../build_modules/pthreads.cmake)

FILE(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cc)
FILE(GLOB TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${GTEST_INCLUDE_DIRS}) 
include_directories(${GMOCK_INCLUDE_DIRS}) 

add_executable(fzdb-test ${TEST_SOURCES} ${TEST_HEADERS} $<TARGET_OBJECTS:gtest> $<TARGET_OBJECTS:gmock> $<TARGET_OBJECTS:fzdb-lib>)
add_dependencies(fzdb-test nowide)
add_dependencies(fzdb-test gtest)

add_dependencies(fzdb-test fzdb-lib)
add_dependencies(fzdb-test spdlog)
add_dependencies(fzdb-test rapidjson)

target_link_libraries(fzdb-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fzdb-test ${Boost_LIBRARIES})
target_link_libraries(fzdb-test ${Boost_Nowide_LIBRARIES})

if(UNIX AND NOT APPLE)
   target_link_libraries(fzdb-test -lrt) 
endif()

add_test(NAME test COMMAND fzdb-test)