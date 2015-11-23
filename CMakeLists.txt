project(fuzzy-database)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

INCLUDE(ExternalProject)

if(UNIX)
  set (CMAKE_CXX_FLAGS -std=c++11)
  set (CMAKE_VERBOSE_MAKEFILE true )
endif()

#Default is DEBUG build
IF(NOT CMAKE_BUILD_TYPE MATCHES "RELEASE")
  message("Assuming debug build, use -DCMAKE_BUILD_TYPE=RELEASE for release build")
  SET(CMAKE_BUILD_TYPE "DEBUG")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  message("Starting DEBUG configuration..")
elseif(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message("Starting RELEASE configuration..")
else()
  message( FATAL_ERROR "CMAKE_BUILD_TYPE must be DEBUG or RELEASE - it's currently ${CMAKE_BUILD_TYPE}" )
endif()

message("Using generator ${CMAKE_GENERATOR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/pthreads.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/boost-nowide.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${Boost_Nowide_INCLUDES}) 
include_directories(${Boost_INCLUDE_DIRS}) 

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp)
FILE(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h)

add_executable(fuzzy-database ${SOURCES} ${HEADERS})

target_link_libraries(fuzzy-database ${Boost_LIBRARIES})
target_link_libraries(fuzzy-database ${Boost_Nowide_LIBRARIES})
target_link_libraries(fuzzy-database ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(fuzzy-database nowide)

if(UNIX AND NOT APPLE)
   target_link_libraries(fuzzy-database -lrt) 
endif()
