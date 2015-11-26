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

set(PD "${CMAKE_CURRENT_SOURCE_DIR}/../src/")

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/**/*.cpp)
SET(SOURCES_NO_MAIN ${SOURCES})
LIST(REMOVE_ITEM SOURCES_NO_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")
#message("BOO: ${SOURCES}")
#message("WOO: ${SOURCES_NO_MAIN}")

add_executable(fuzzy-database-test ${TEST_SOURCES} ${SOURCES_NO_MAIN} ${TEST_HEADERS} $<TARGET_OBJECTS:gtest>)
add_dependencies(fuzzy-database-test nowide)
add_dependencies(fuzzy-database-test gtest)

#remove main otherwise there are 2 main functions!

target_link_libraries(fuzzy-database-test ${GTEST_LIBRARIES})
target_link_libraries(fuzzy-database-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fuzzy-database-test ${Boost_LIBRARIES})
target_link_libraries(fuzzy-database-test ${Boost_Nowide_LIBRARIES})

add_test(NAME test COMMAND fuzzy-database-test)
