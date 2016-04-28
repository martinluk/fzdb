project(fzdb-service)
cmake_minimum_required(VERSION 2.8.7)
cmake_policy(VERSION 2.8.7)

message("Building Windows Service")

include(${CMAKE_CURRENT_SOURCE_DIR}/../build_modules/pthreads.cmake)

FILE(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cc)
FILE(GLOB TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_executable(fzdb-service ${TEST_SOURCES} ${TEST_HEADERS} $<TARGET_OBJECTS:fzdb-lib>)
add_dependencies(fzdb-service nowide)

add_dependencies(fzdb-service fzdb-lib)
add_dependencies(fzdb-service spdlog)
add_dependencies(fzdb-service rapidjson)

target_link_libraries(fzdb-service ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fzdb-service ${Boost_LIBRARIES})
target_link_libraries(fzdb-service ${Boost_Nowide_LIBRARIES})

if(UNIX AND NOT APPLE)
   target_link_libraries(fzdb-service -lrt) 
endif()