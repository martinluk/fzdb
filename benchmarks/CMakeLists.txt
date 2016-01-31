project(fuzzy-database-benchmark)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

message("Building Benchmarks")

include(${CMAKE_CURRENT_SOURCE_DIR}/../build_modules/pthreads.cmake)

FILE(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp)
FILE(GLOB TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${BENCHMARK_INCLUDE_DIRS}) 

set(PD "${CMAKE_CURRENT_SOURCE_DIR}/../src/")

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/**/*.cpp)
SET(SOURCES_NO_MAIN ${SOURCES})
LIST(REMOVE_ITEM SOURCES_NO_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")
#message("BOO: ${SOURCES}")
#message("WOO: ${SOURCES_NO_MAIN}")

add_executable(fuzzy-database-benchmark ${TEST_SOURCES} ${SOURCES_NO_MAIN} ${TEST_HEADERS})
add_dependencies(fuzzy-database-benchmark nowide)
add_dependencies(fuzzy-database-benchmark benchmark)
add_dependencies(fuzzy-database-benchmark spdlog)
add_dependencies(fuzzy-database-benchmark rapidjson)
add_dependencies(fuzzy-database-benchmark benchmark)

#remove main otherwise there are 2 main functions!

#target_link_libraries(fuzzy-database-test ${GTEST_LIBRARIES})
target_link_libraries(fuzzy-database-benchmark ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fuzzy-database-benchmark ${Boost_LIBRARIES})
target_link_libraries(fuzzy-database-benchmark ${Boost_Nowide_LIBRARIES})
if(MSVC)
  target_link_libraries(fuzzy-database-benchmark "../benchmark-prefix/src/benchmark-build/src/Debug/benchmark")
elseif(GCC)
  target_link_libraries(fuzzy-database-benchmark "../benchmark-prefix/src/benchmark-build/src/libbenchmark")
endif()
if(WIN32)
target_link_libraries(fuzzy-database-benchmark ${SHLWAPI})
endif()

#add_test(NAME benchmark COMMAND fuzzy-database-benchmark)
